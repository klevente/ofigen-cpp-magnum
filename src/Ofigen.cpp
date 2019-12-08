#include <string>
#include <vector>
#include <random>
#include <map>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iomanip>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

#include <Magnum/Shaders/Flat.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/Image.h>

#include <json.hpp>
#include <png++/png.hpp>

#include "ColoredDrawable.h"
#include "TexturedDrawable.h"
#include "BackgroundDrawable.h"
#include "MovingObject.h"
#include "ObjectData.h"

using namespace Magnum;
using namespace Math::Literals;

using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;
using json = nlohmann::json;

class Ofigen : public Platform::Application {
public:
    explicit Ofigen(const Arguments& arguments);

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent& event) override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    [[nodiscard]] Vector3 positionOnSphere(const Vector2i& position) const;

    void loadConfig(const std::string & filename);
    void loadMeshesFromFolder(const std::string & foldername);
    void loadMesh(Containers::Pointer<Trade::AbstractImporter> & importer, const std::string & filename);

    void addObjectToScene(const std::string & name, const Vector3 & position = {});
    void loadBackground(const std::string & filename);
    void addBackgroundToScene(const Vector3 & position = {});
    void moveObjects();
    void writeImage();

    void exitEvent(ExitEvent &) override;

    void keyPressEvent(KeyEvent &event) override;

    Vector3 _minPoint, _maxPoint;
    unsigned int _numOfObjects;
    unsigned int _numOfIterations;
    std::string _outFolderName;
    std::string _outFileName;

    unsigned int _currentIteration = 0;

    Shaders::Phong _coloredShader,
            _texturedShader{Shaders::Phong::Flag::DiffuseTexture};
    Containers::Array<Containers::Optional<GL::Mesh>> _meshes;
    Containers::Array<Containers::Optional<GL::Texture2D>> _textures;

    std::vector<std::string> _filenames;
    std::map<std::string, Containers::Array<Containers::Optional<GL::Mesh>>> _meshMap;
    std::map<std::string, Containers::Array<Containers::Optional<GL::Texture2D>>> _texturesMap;
    std::map<std::string, Containers::Array<Containers::Optional<Trade::PhongMaterialData>>> _materialsMap;
    std::map<std::string, ObjectData> _objectDataMap;

    Scene3D _scene;
    MovingObject _manipulator, _cameraObject;
    SceneGraph::Camera3D* _camera;
    SceneGraph::DrawableGroup3D _drawables;
    std::vector<MovingObject *> _objects;
    Vector3 _previousPosition;

    Shaders::Flat3D _backgroundShader{Shaders::Flat3D::Flag::Textured};
    GL::Mesh _backgroundMesh;
    GL::Texture2D _backgroundTexture;
    MovingObject _backgroundObject;
};

Ofigen::Ofigen(const Arguments& arguments):
        Platform::Application{arguments, Configuration{}
                .setTitle("Magnum Viewer Example")
                .setWindowFlags(Configuration::WindowFlag::Resizable)},
                _manipulator{_scene}, _cameraObject{_scene},
                _backgroundObject{_scene}
{
    Utility::Arguments args;
    args.addArgument("config").setHelp("config", "Config file to load")
            .addSkippedPrefix("magnum", "engine-specific options")
            .setGlobalHelp("Displays a 3D scene file provided on command line.")
            .parse(arguments.argc, arguments.argv);

    _cameraObject.move(Vector3::zAxis(50.0f));

    (*(_camera = new SceneGraph::Camera3D{_cameraObject}))
            .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());

    /* Base object, parent of all (for easy manipulation) */
    _manipulator.setParent(&_scene);

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    _coloredShader
            .setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);
    _texturedShader
            .setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);

    loadConfig(args.value("config"));

    std::filesystem::create_directories(_outFolderName);

    addBackgroundToScene({0, 0, -100});

    std::mt19937 mt{std::random_device{}()};
    std::uniform_real_distribution<float> xPos{_minPoint.x(), _maxPoint.x()};
    std::uniform_real_distribution<float> yPos{_minPoint.y(), _maxPoint.y()};
    std::uniform_real_distribution<float> zPos{_minPoint.z(), _maxPoint.z()};

    std::uniform_int_distribution<unsigned int> index{0, _filenames.size() - 1};

    for (unsigned int i = 0; i < _numOfObjects; i++) {
        addObjectToScene(_filenames[index(mt)], {xPos(mt), yPos(mt), zPos(mt)});
    }
}

void Ofigen::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth)
        .bind();

    moveObjects();
    _camera->draw(_drawables);
    swapBuffers();
    writeImage();
    _currentIteration++;
    if (_currentIteration > _numOfIterations) {
        std::exit(0);
    }

    redraw();
}

void Ofigen::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    _camera->setViewport(event.windowSize());
}

void Ofigen::mousePressEvent(MouseEvent& event) {
    if(event.button() == MouseEvent::Button::Left)
        _previousPosition = positionOnSphere(event.position());
}

void Ofigen::mouseReleaseEvent(MouseEvent& event) {
    if(event.button() == MouseEvent::Button::Left)
        _previousPosition = Vector3();
}

void Ofigen::mouseScrollEvent(MouseScrollEvent& event) {
    if(!event.offset().y()) return;

    /* Distance to origin */
    const Float distance = _cameraObject.transformation().translation().z();

    /* Move 15% of the distance back or forward */
    _cameraObject.translate(Vector3::zAxis(
            distance*(1.0f - (event.offset().y() > 0 ? 1/0.85f : 0.85f))));

    redraw();
}

Vector3 Ofigen::positionOnSphere(const Vector2i& position) const {
    const Vector2 positionNormalized = Vector2{position}/Vector2{_camera->viewport()} - Vector2{0.5f};
    const Float length = positionNormalized.length();
    const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
    return (result*Vector3::yScale(-1.0f)).normalized();
}

void Ofigen::mouseMoveEvent(MouseMoveEvent& event) {
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    const Vector3 currentPosition = positionOnSphere(event.position());
    const Vector3 axis = Math::cross(_previousPosition, currentPosition);

    if(_previousPosition.length() < 0.001f || axis.length() < 0.001f) return;

    _manipulator.rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
    _previousPosition = currentPosition;

    redraw();
}

void Ofigen::loadConfig(const std::string & filename) {
    std::ifstream i{filename};
    json j;
    i >> j;

    _numOfIterations = j["num_of_iterations"].get<unsigned int>();
    auto backgroundName = j["background_name"].get<std::string>();
    loadBackground(backgroundName);
    auto modelFolder = j["models_folder"].get<std::string>();
    loadMeshesFromFolder(modelFolder);
    _numOfObjects = j["num_of_objects"].get<unsigned int>();

    static const char * objPlacementMin = "object_placement_min";
    static const char * objPlacementMax = "object_placement_max";

    _minPoint = {
            j[objPlacementMin]["x"].get<float>(),
            j[objPlacementMin]["y"].get<float>(),
            j[objPlacementMin]["z"].get<float>()
    };

    _maxPoint = {
            j[objPlacementMax]["x"].get<float>(),
            j[objPlacementMax]["y"].get<float>(),
            j[objPlacementMax]["z"].get<float>()
    };

    static const char * moveConstraintStr = "object_move_constraint";
    static const char * rotConstraintStr = "object_rot_constraint";

    float moveCoeff{ j["object_move_coeff"].get<float>() };
    Vector3 moveConstraint{
        j[moveConstraintStr]["x"].get<float>(),
        j[moveConstraintStr]["y"].get<float>(),
        j[moveConstraintStr]["z"].get<float>()
    };

    float rotCoeff{ j["object_rot_coeff"].get<float>() };
    Vector3 rotConstraint{
            j[rotConstraintStr]["x"].get<float>(),
            j[rotConstraintStr]["y"].get<float>(),
            j[rotConstraintStr]["z"].get<float>()
    };

    MovingObject::initCoefficients(moveCoeff, rotCoeff, moveConstraint, rotConstraint);

    _outFolderName = j["out_folder"].get<std::string>();
    _outFileName = j["out_file"].get<std::string>();
}

void Ofigen::loadMeshesFromFolder(const std::string & foldername) {
    /* Load a scene importer plugin */
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AnySceneImporter");
    if(!importer) {
        std::exit(1);
    }

    namespace fs = std::filesystem;
    const fs::path folder{foldername};
    for (const auto & file : fs::directory_iterator{folder}) {
        if (!file.is_directory()) {
            const auto & filename = file.path().string();
            _filenames.push_back(filename);
            loadMesh(importer, filename);
        }
    }
}

void Ofigen::loadMesh(Containers::Pointer<Trade::AbstractImporter> & importer, const std::string & filename) {
    Debug{} << "Opening file" << filename;
    /* Load file */
    if (!importer->openFile(filename)) {
        std::exit(4);
    }

    const auto [textureIterator, textureSuccess] = _texturesMap.try_emplace(filename, importer->textureCount());
    for (UnsignedInt i = 0; i < importer->textureCount(); i++) {
        Debug{} << "Importing texture" << i << importer->textureName(i);

        Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
        if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
            Warning{} << "Cannot load texture properties, skipping";
            continue;
        }

        Debug{} << "Importing image" << textureData->image() << importer->image2DName(textureData->image());
        Containers::Optional<Trade::ImageData2D> imageData = importer->image2D(textureData->image());
        GL::TextureFormat format;
        if (imageData && imageData->format() == PixelFormat::RGB8Unorm)
            format = GL::TextureFormat::RGB8;
        else if (imageData && imageData->format() == PixelFormat::RGBA8Unorm)
            format = GL::TextureFormat::RGBA8;
        else {
            Warning{} << "Cannot load texture image, skipping";
            continue;
        }

        /* Configure the texture */
        GL::Texture2D texture;
        texture
                .setMagnificationFilter(textureData->magnificationFilter())
                .setMinificationFilter(textureData->minificationFilter(), textureData->mipmapFilter())
                .setWrapping(textureData->wrapping().xy())
                .setStorage(Math::log2(imageData->size().max()) + 1, format, imageData->size())
                .setSubImage(0, {}, *imageData)
                .generateMipmap();

        textureIterator->second[i] = std::move(texture);
    }

    /* Load all materials. Materials that fail to load will be NullOpt. */
    const auto [materialIterator, materialSuccess] = _materialsMap.try_emplace(filename, importer->materialCount());
    for (UnsignedInt i = 0; i < importer->materialCount(); i++) {
        Debug{} << "Importing material" << i << importer->materialName(i);

        Containers::Pointer<Trade::AbstractMaterialData> materialData = importer->material(i);
        if (!materialData || materialData->type() != Trade::MaterialType::Phong) {
            Warning{} << "Cannot load material, skipping";
            continue;
        }

        materialIterator->second[i] = std::move(static_cast<Trade::PhongMaterialData&>(*materialData));
    }

    const auto [meshIterator, meshSuccess] = _meshMap.try_emplace(filename, importer->mesh3DCount());
    for (UnsignedInt i = 0; i < importer->mesh3DCount(); i++) {
        Debug{} << "Importing mesh" << i << importer->mesh3DName(i);
        Containers::Optional<Trade::MeshData3D> meshData = importer->mesh3D(i);
        if(!meshData || !meshData->hasNormals() || meshData->primitive() != MeshPrimitive::Triangles) {
            Warning{} << "Cannot load the mesh, skipping";
            continue;
        }

        /* Compile the mesh */
        meshIterator->second[i] = MeshTools::compile(*meshData);
    }

    /* Load the scene */
    if (importer->defaultScene() != -1) {
        Debug{} << "Adding default scene" << importer->sceneName(importer->defaultScene());

        Containers::Optional<Trade::SceneData> sceneData = importer->scene(importer->defaultScene());
        if (!sceneData) {
            Error{} << "Cannot load scene, exiting";
            return;
        }

        const auto [objectDataIterator, objectDataSuccess] = _objectDataMap.try_emplace(filename);
        ObjectData & objectVector = objectDataIterator->second;

        std::function<void(UnsignedInt)> addChildObjects = [&](UnsignedInt objectId) -> void {
            Debug{} << "Importing child object" << objectId << importer->object3DName(objectId);
            // maybe std::move object3d?
            objectVector.childrenObjects.emplace_back(std::move(importer->object3D(objectId)));
            if (!objectVector.childrenObjects.back()) {
                Error{} << "Cannot import child object, skipping";
                return;
            }
            for (UnsignedInt childId : objectVector.childrenObjects.back()->children()) {
                addChildObjects(childId);
            }
        };

        for (UnsignedInt objectId : sceneData->children3D()) {
            Debug{} << "Importing root object" << objectId << importer->object3DName(objectId);
            objectVector.rootObjects.emplace_back(std::move(importer->object3D(objectId)));
            if (!objectVector.rootObjects.back()) {
                Error{} << "Cannot import child object, skipping";
                continue;
            }
            Debug{} << "Adding child objects:" << objectVector.rootObjects.back()->children().size();
            for (UnsignedInt childId : objectVector.rootObjects.back()->children()) {
                addChildObjects(childId);
            }
        }
    }
}

void Ofigen::addObjectToScene(const std::string & name, const Vector3 & position) {
    Debug{} << "Adding" << name << "to scene";

    auto & meshes = _meshMap[name];
    auto & textures = _texturesMap[name];
    auto & materials = _materialsMap[name];
    auto & objectDatas = _objectDataMap[name];

    std::function<void(Containers::Pointer<Trade::ObjectData3D> &, MovingObject *)> addSubObjectToScene =
            [&] (Containers::Pointer<Trade::ObjectData3D> & objectData, MovingObject * parent) {

        MovingObject * object = parent;
        /* Add a drawable if the object has a mesh and the mesh is loaded */
        if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() !=  -1 && meshes[objectData->instance()]) {
            object = new MovingObject{parent};
            auto materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

            /* Material not available / not loaded, use a default material */
            if (materialId == -1 || !materials[materialId]) {
                new ColoredDrawable{*object, _coloredShader, *meshes[objectData->instance()], 0xffffff_rgbf, _drawables};
            /* Textured material. If the texture failed to load, again just use a
           default colored material. */
            } else if (materials[materialId]->flags() & Trade::PhongMaterialData::Flag::DiffuseTexture) {
                auto & texture = textures[materials[materialId]->diffuseTexture()];

                if (texture) {
                    new TexturedDrawable{*object, _texturedShader, *meshes[objectData->instance()], *texture, _drawables};
                } else {
                    new ColoredDrawable{*object, _coloredShader, *meshes[objectData->instance()], 0xffffff_rgbf, _drawables};
                }
            }
        }

        for (auto id : objectData->children()) {
            auto & child = objectDatas.childrenObjects[id];
            addSubObjectToScene(child, object);
        }
    };

    auto * manipulatorObject = new MovingObject{&_manipulator};
    manipulatorObject->move(position);
    _objects.push_back(manipulatorObject);
    manipulatorObject->setName(name + '_' + std::to_string(_objects.size() - 1));
    for (auto & od : objectDatas.rootObjects) {
        addSubObjectToScene(od, manipulatorObject);
    }


}

void Ofigen::exitEvent(Platform::Sdl2Application::ExitEvent &) {
    std::exit(0);
}

void Ofigen::loadBackground(const std::string & filename) {
    Debug{} << "Loading background image";
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AnyImageImporter");
    if (!importer) {
        std::exit(1);
    }
    if (!importer->openFile(filename)) {
        std::exit(4);
    }

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_INTERNAL_ASSERT(image);
    Debug{} << "Importing background" << importer->image2DName(0);
    _backgroundTexture.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setStorage(1, GL::textureFormat(image->format()), image->size())
        .setSubImage(0, {}, *image);

    _backgroundMesh = MeshTools::compile(Primitives::planeSolid(Primitives::PlaneTextureCoords::Generate));
}

void Ofigen::addBackgroundToScene(const Vector3 &position) {
    Debug{} << "Adding background to scene";
    _backgroundObject.scale({100, 100, 1});
    _backgroundObject.move(position);
    new BackgroundDrawable{_backgroundObject, _backgroundShader, _backgroundMesh, _backgroundTexture, _drawables};

}

void Ofigen::keyPressEvent(Platform::Sdl2Application::KeyEvent &event) {
    if (event.key() == KeyEvent::Key::Space) {
        Magnum::Image2D image{PixelFormat::RGB8Unorm};

        _camera->draw(_drawables);

        GL::defaultFramebuffer.read(GL::defaultFramebuffer.viewport(), image);

        png::image<png::rgb_pixel> pngImage{static_cast<unsigned int>(GL::defaultFramebuffer.viewport().sizeX()), static_cast<unsigned int>(GL::defaultFramebuffer.viewport().sizeY())};

        auto pixels = image.pixels<Color3ub>();

        std::size_t x = 0, y = 0;
        for (auto row : pixels) {
            for (auto & pixel : row) {
                // Debug{} << pixel;
                auto yInv = pngImage.get_height() - 1 - y;
                pngImage.set_pixel(x, yInv, png::rgb_pixel{pixel.r(), pixel.g(), pixel.b()});
                x++;
            }
            y++;
            x = 0;
        }

        pngImage.write("out.png");
    }
    if (event.key() == KeyEvent::Key::M) {
        moveObjects();
        redraw();
    }
}

void Ofigen::moveObjects() {
    json jsonData;
    jsonData["iteration"] = _currentIteration;
    jsonData["objectData"] = json::array();
    for (const auto & obj : _objects) {
        obj->moveRandomly(jsonData["objectData"]);
    }

    std::ofstream ofstream{_outFolderName + '\\' + _outFileName + '_' + std::to_string(_currentIteration) + ".json"};

    ofstream << std::setw(4) << jsonData;
}

void Ofigen::writeImage() {
    Magnum::Image2D image{PixelFormat::RGB8Unorm};

    GL::defaultFramebuffer.read(GL::defaultFramebuffer.viewport(), image);
    png::image<png::rgb_pixel> pngImage{static_cast<unsigned int>(GL::defaultFramebuffer.viewport().sizeX()), static_cast<unsigned int>(GL::defaultFramebuffer.viewport().sizeY())};
    auto pixels = image.pixels<Color3ub>();

    std::size_t x = 0, y = 0;
    for (auto row : pixels) {
        for (auto & pixel : row) {
            auto yInv = pngImage.get_height() - 1 - y;
            pngImage.set_pixel(x, yInv, png::rgb_pixel{pixel.r(), pixel.g(), pixel.b()});
            x++;
        }
        y++;
        x = 0;
    }

    pngImage.write(_outFolderName + '\\' + _outFileName + '_' + std::to_string(_currentIteration) + ".png");
}

MAGNUM_APPLICATION_MAIN(Ofigen)
