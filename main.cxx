#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>

namespace ChunkId {
    constexpr uint16_t Main = 0x4d4d;
    constexpr uint16_t Version = 0x0002;
    constexpr uint16_t Editor = 0x3d3d;
    constexpr uint16_t Object = 0x4000;
    constexpr uint16_t TriangularMesh = 0x4100;
    constexpr uint16_t VerticesList = 0x4110;
    constexpr uint16_t FacesDescription = 0x4120;
    constexpr uint16_t MappingCoordinatesList = 0x4140;
};

constexpr uint8_t MAX_NAME_LENGTH = 20;
constexpr size_t headerSize = sizeof(uint16_t) + sizeof(uint32_t);

int main(int argc, char* argv[])
{
    const char* c_filename = "consoleboat-opt2.3ds";
    std::ifstream modelFile {c_filename, std::ifstream::binary};

    if (!modelFile.is_open()) {
        printf("Failed to open %s", c_filename);
        return 1;
    }

    bool done = false;
    bool verticesParsed = false;
    bool facesParsed = false;
    bool uvsParsed = false;
    while (!modelFile.eof() && !done) {
        uint16_t id {0};
        uint32_t size {0};
        modelFile.read(reinterpret_cast<char *>(&id), sizeof(uint16_t));
        modelFile.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));

        switch (id) {
            case ChunkId::Main:
                break;
            /* case ChunkId::Version:
                break; */
            case ChunkId::Editor:
                break;
            case ChunkId::Object:
                char name[MAX_NAME_LENGTH];
                modelFile.getline(name, MAX_NAME_LENGTH, '\0');
                printf("%s\n", name);
                break;
            case ChunkId::TriangularMesh:
                break;
            case ChunkId::VerticesList:
                {
                    uint16_t vertCount {0};
                    modelFile.read(reinterpret_cast<char *>(&vertCount), sizeof(uint16_t));
                    struct Vertex { float x,y,z; } v{};
                    printf("Vertices\n");
                    for (uint16_t i = 0; i < vertCount; ++i) {
                        modelFile.read(reinterpret_cast<char *>(&v), sizeof(Vertex));
                        printf("V: %f, %f, %f\n", v.x, v.y, v.z);
                    }
                    verticesParsed = true;
                    printf("Found %d vertices\n\n", vertCount);
                }
                break;
            case ChunkId::FacesDescription:
                {
                    uint16_t faceCount {0};
                    modelFile.read(reinterpret_cast<char *>(&faceCount), sizeof(uint16_t));
                    struct Face { uint16_t a, b, c; uint16_t flags; } f{};
                    printf("Faces\n");
                    for (uint16_t i = 0; i < faceCount; ++i) {
                        modelFile.read(reinterpret_cast<char *>(&f), sizeof(Face));
                        printf("F: %d, %d, %d\n", f.a, f.b, f.c);
                    }
                    facesParsed = true;
                    printf("Found %d faces\n\n", faceCount);
                }
                break;
            case ChunkId::MappingCoordinatesList:
                {
                    uint16_t uvCount {0};
                    modelFile.read(reinterpret_cast<char *>(&uvCount), sizeof(uint16_t));
                    struct UV { float u,v; } uv{};
                    printf("Texture Coordinates\n");
                    for (uint16_t i = 0; i < uvCount; ++i) {
                        modelFile.read(reinterpret_cast<char *>(&uv), sizeof(UV));
                        printf("T: %f, %f\n", uv.u, uv.v);
                    }
                    uvsParsed = true;
                    printf("Found %d uvs\n\n", uvCount);
                }
                break;
            default:
                // Skip what I don't know or need
                modelFile.seekg(size - headerSize, modelFile.cur);
                break;
        }

        done = verticesParsed && facesParsed && uvsParsed;
    }

    return 0;
}

