#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../libraries/stb/stb_image.h"

// 引数で渡されたパスからディレクトリ部分を抽出する関数
std::string getDirectoryPath(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) return path.substr(0, pos + 1);
    else return "";
}

// 頂点、法線、テクスチャ座標の構造体
struct Vertex {
    float x, y, z;
};

struct Normal {
    float nx, ny, nz;
};

struct TexCoord {
    float u, v;
};

// 面の構造体
struct Face {
    std::vector<int> vertexIndices;
    std::vector<int> texCoordIndices;
    std::vector<int> normalIndices;

    std::string materialName;
};

// マテリアルの構造体
struct Material {
    std::string name;

    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;

    unsigned int textureID;
};

std::vector<Vertex>   vertices;
std::vector<Normal>   normals;
std::vector<TexCoord> texCoords;
std::vector<Face>     faces;

std::map<std::string, Material>     materials;
std::map<std::string, unsigned int> textures;

bool isTextureDrawing = true;

// mtlファイルを読み込む関数
bool loadMTL(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << path << std::endl;
        return false;
    }

    Material* currentMaterial = nullptr;

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "newmtl") {
            std::string name;
            iss >> name;

            currentMaterial = &materials[name];
            currentMaterial->name = name;
        }

        // 環境光
        else if (type == "Ka") {
            if (currentMaterial) {
                iss >> currentMaterial->ambient[0] >> currentMaterial->ambient[1]
                    >> currentMaterial->ambient[2] >> currentMaterial->ambient[3];
            }
        }

        // 拡散光
        else if (type == "Kd") {
            if (currentMaterial) {
                iss >> currentMaterial->diffuse[0] >> currentMaterial->diffuse[1]
                    >> currentMaterial->diffuse[2] >> currentMaterial->diffuse[3];
            }
        }

        // 反射光
        else if (type == "Ks") {
            if (currentMaterial) {
                iss >> currentMaterial->specular[0] >> currentMaterial->specular[1]
                    >> currentMaterial->specular[2] >> currentMaterial->specular[3];
            }
        }

        // 光沢度
        else if (type == "Ns") {
            if (currentMaterial) {
                iss >> currentMaterial->shininess;
            }
        }

        // 拡散光のテクスチャ
        else if (type == "map_Kd") {
            std::string textureFile;
            iss >> textureFile;

            if (currentMaterial) {
                GLuint textureID;
                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_2D, textureID);

                stbi_set_flip_vertically_on_load(true); // 上下逆にする

                int width, height, channels;

                unsigned char* image = stbi_load((getDirectoryPath(path) + textureFile).c_str(), &width, &height, &channels, 0);

                if (image) {
                    int format = (channels == 4) ? GL_RGBA : GL_RGB;

                    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

                    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

                    stbi_image_free(image);

                    currentMaterial->textureID = textureID;
                }
                else {
                    std::cerr << "Failed to load texture: " << textureFile << std::endl;
                }
            }
        }
    }

    file.close();

    return true;
}

// objファイルを読み込む関数
bool loadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<Normal>& normals, std::vector<TexCoord>& texCoords, std::vector<Face>& faces) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::string line;
    std::string currentMaterial = "default";

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            Vertex vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (type == "vt") {
            TexCoord texCoord;
            iss >> texCoord.u >> texCoord.v;
            texCoords.push_back(texCoord);
        }
        else if (type == "vn") {
            Normal normal;
            iss >> normal.nx >> normal.ny >> normal.nz;
            normals.push_back(normal);
        }
        else if (type == "f") {
            Face face;

            std::string vertexData;

            while (iss >> vertexData) {
                std::istringstream viss(vertexData);

                std::string index;

                int vIndex = -1, vtIndex = -1, vnIndex = -1;
                int i = 0;

                while (std::getline(viss, index, '/')) {
                    if (!index.empty()) {
                        int idx = std::stoi(index) - 1;

                        if (i == 0) vIndex = idx;
                        else if (i == 1) vtIndex = idx;
                        else if (i == 2) vnIndex = idx;
                    }
                    i++;
                }

                face.vertexIndices.push_back(vIndex);
                face.texCoordIndices.push_back(vtIndex);
                face.normalIndices.push_back(vnIndex);
            }

            // マテリアルを適用
            face.materialName = currentMaterial;
            faces.push_back(face);
        }
        else if (type == "usemtl") {
            iss >> currentMaterial;
        }
        else if (type == "mtllib") {
            std::string mtlFile;
            iss >> mtlFile;

            // mtlファイルをロード
            if (!loadMTL(getDirectoryPath(path) + mtlFile)) {
                std::cerr << "Failed to load MTL file: " << mtlFile << std::endl;
            }
        }
    }

    file.close();

    return true;
}

// objファイルのモデルを描画する関数
void drawOBJ(const std::vector<Vertex>& vertices, const std::vector<Normal>& normals, const std::vector<TexCoord>& texCoords, const std::vector<Face>& faces) {
    for (const auto& face : faces) {
        const Material& mat = materials[face.materialName];

        if (mat.textureID != 0 && isTextureDrawing) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, mat.textureID);
        }
        else {
            glDisable(GL_TEXTURE_2D);
            glColor4fv(mat.diffuse);
        }

        glBegin(GL_TRIANGLES);

        for (int i = 0; i < 3; i++) {
            if (!face.normalIndices.empty()) {
                const Normal& n = normals[face.normalIndices[i]];
                glNormal3f(n.nx, n.ny, n.nz);
            }

            if (!face.texCoordIndices.empty()) {
                const TexCoord& t = texCoords[face.texCoordIndices[i]];
                glTexCoord2f(t.u, t.v);
            }

            const Vertex& v = vertices[face.vertexIndices[i]];
            glVertex3f(v.x, v.y, v.z);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D); // テクスチャ使用後に無効化
    }
}

float angleX = 0.0f;  // X軸回転角度
float angleY = 0.0f;  // Y軸回転角度
float zoom = -5.0f; // ズームの距離（視点からの距離）

int lastX = 0, lastY = 0; // 最後のマウス位置
bool isDragging = false;  // ドラッグ状態かどうか

// オブジェクトモデルの描画
void drawObjects() {
    glPushMatrix();

    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    //glColor3f(1.0f, 0.0f, 0.0f); // オブジェクトの色（赤）

    //glutSolidTeapot(1.0);

    drawOBJ(vertices, normals, texCoords, faces);

    glPopMatrix();
}

// マウス モーション イベント
void mouseMotion(int x, int y) {
    // マウスの動きによる視点移動
    if (isDragging) {
        // マウスを上下に動かすことでズーム
        int deltaY = y - lastY;
        zoom += deltaY * 0.05f;             // ズーム感度を調整
        if (zoom > -1.0f)   zoom = -1.0f;   // ズームの最小値を制限
        if (zoom < -100.0f) zoom = -100.0f; // ズームの最大値を制限
        lastY = y;
    }
    else {
        int dx = x - lastX;  // 横の動き
        int dy = y - lastY;  // 縦の動き

        // マウスの動きに応じて回転角度を調整
        angleX += dy * 0.2f; // 縦方向の回転
        angleY += dx * 0.2f; // 横方向の回転

        lastX = x;
        lastY = y;
    }

    glutPostRedisplay(); // 再描画
}

// マウス ボタン イベント（ボタン押下）
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // 左ボタンが押された時、ドラッグ開始
            isDragging = true;
            lastY = y;
        }
        else if (state == GLUT_UP) {
            // 左ボタンが離された時、ドラッグ終了
            isDragging = false;
        }
    }

    if (state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
    }
}

// キーボード イベント
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 't':
        if (isTextureDrawing) isTextureDrawing = false;
        else isTextureDrawing = true;
        glutPostRedisplay();
        break;

    case '0':
        exit(0);
        break;

    default:
        break;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 視点の回転
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(angleX, 1.0f, 0.0f, 0.0f); // X軸回転
    glRotatef(angleY, 0.0f, 1.0f, 0.0f); // Y軸回転

    drawObjects();

    glutSwapBuffers();
}

void initialize() {
    glEnable(GL_DEPTH_TEST); // 深度テストを有効化
    glEnable(GL_LIGHTING);   // ライティングを有効化
    glEnable(GL_LIGHT0);     // 光源0を有効化

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 背景色設定

    // カメラ設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("OpenGL - Model Viewer");

    initialize();

    const char* modelPath = "";
    if (argc > 1) modelPath = argv[1];

    if (!loadOBJ(modelPath, vertices, normals, texCoords, faces)) return -1;

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);
    glutMainLoop();

    return 0;
}