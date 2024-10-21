#include <stdio.h>
#include <math.h>

#include <time.h>   // 日付時刻関連
#include <iostream> // 入出力関連
#include <fstream>  // ファイル操作関連
#include <string>   // 文字列操作関連
#include <map>      // 連想配列
#include <vector>   // 動的配列（要素を追加したり削除したり）
#include <sstream>  // 文字列ストリーム操作（文字列から数値への変換や数値から文字列への変換）

#define _USE_MATH_DEFINES
using std::string;
using std::to_string;

#include "windows.h"

//////////////////////////////////////////////////

#include <GL/gl.h>
#include <GL/glut.h>

//////////////////////////////////////////////////

#define COUNTOF(array) (sizeof(array) / sizeof(array[0])) // 配列の要素数を判定
#define CHAR_TO_INT(c) ((c) - '0')

//////////////////////////////////////////////////

int windowPositionX = 100;
int windowPositionY = 100;
int windowWidth     = 512;
int windowHeight    = 512;

char windowTitle[] = "objファイルからモデルをロード";

////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> getFileContents(const char* filePath) {
    std::vector<std::string> stringArray;

    std::ifstream file(filePath);

    if (!file.is_open()) throw;

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        else stringArray.push_back(line);
    }

    file.close();

    return stringArray;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

GLdouble vertex[100][3]; // 頂点座標
GLdouble uv[100][2];     // テクスチャ座標
GLdouble normal[100][3]; // 法線ベクトル
int      face[100][9];   // 面の情報

void parseObjFileData(std::vector<std::string> objFileData) {
    std::string valueA, valueB, valueC, valueD; // 行からスペースで区切られた文字列を順に格納

    std::string key;
    float x, y, z;

    std::string indices[3]; // fキーの3つのインデックスを格納

    int v = 1; // 頂点座標 の数
    int u = 1; // テクスチャ座標 の数
    int n = 1; // 法線ベクトル の数
    int f = 0; // 面 の数

    int digitCounter = 1;
    int slashCounter = 0;
    int currentIndex = 0;

    int vIndices[3] = { 0 };
    int uIndices[3] = { 0 };
    int nIndices[3] = { 0 };

    int currentFaceIndex = 0;

    // 頂点座標、テクスチャ座標、法線ベクトル、面の解析
    for (int i = 0; i < objFileData.size(); i++) {

        std::istringstream iss(objFileData[i]);
        iss >> valueA >> valueB >> valueC >> valueD;

        key = valueA;

        x = std::stof(valueB);
        y = std::stof(valueC);
        z = std::stof(valueD);

        indices[0] = valueB;
        indices[1] = valueC;
        indices[2] = valueD;

        // 頂点座標
        if (key == "v") {
            vertex[v][0] = x;
            vertex[v][1] = y;
            vertex[v][2] = z;
            v++;
        }

        // テクスチャ座標
        else if (key == "vt") {
            uv[u][0] = x;
            uv[u][1] = y;
            u++;
        }

        // 法線ベクトル
        else if (key == "vn") {
            normal[n][0] = x;
            normal[n][1] = y;
            normal[n][2] = z;
            n++;
        }

        // ポリゴン定義（インデックス）
        else if (key == "f") {

            // 1つの頂点情報に含まれる数字（座標番号、テクスチャ番号、法線番号）を解析
            for (int j = 0; j < (int)indices[currentIndex].size(); j++) {

                if (indices[currentIndex][j] == '/') {
                    slashCounter++;
                    digitCounter = 1;
                }

                if (slashCounter == 0) {
                    if (digitCounter >= 2) {
                        vIndices[currentIndex] *= 10;
                        vIndices[currentIndex] += CHAR_TO_INT(indices[currentIndex][j]);
                        digitCounter++;
                    }
                    else {
                        vIndices[currentIndex] += CHAR_TO_INT(indices[currentIndex][j]);
                        digitCounter *= 10;
                    }
                }

                if (slashCounter == 1) {
                    if (digitCounter >= 2) {
                        uIndices[currentIndex] *= 10;
                        uIndices[currentIndex] += CHAR_TO_INT(indices[currentIndex][j]);
                        digitCounter++;
                    }
                    else {
                        j++;
                        uIndices[currentIndex] += CHAR_TO_INT(indices[currentIndex][j]);
                        digitCounter *= 10;
                    }
                }

                if (slashCounter == 2) {
                    if (digitCounter >= 2) {
                        nIndices[currentIndex] *= 10;
                        nIndices[currentIndex] += CHAR_TO_INT(indices[currentIndex][j]);
                        digitCounter++;
                    }
                    else {
                        j++;
                        nIndices[currentIndex] += CHAR_TO_INT(indices[currentIndex][j]);
                        digitCounter *= 10;
                    }
                }

                j++;
                if (j == (int)indices[currentIndex].size()) {
                    currentIndex++;
                    if (currentIndex == 3) break;
                    j = 0;
                    slashCounter = 0;
                    digitCounter = 1;
                }
                j--;
            }

            for (int j = 0; j < 3; j++) {
                face[f][currentFaceIndex]     = vIndices[j];
                face[f][currentFaceIndex + 1] = uIndices[j];
                face[f][currentFaceIndex + 2] = nIndices[j];

                currentFaceIndex += 3;
            }

            digitCounter = 1;

            for (int j = 0; j < 3; j++) {
                vIndices[j] = 0;
                uIndices[j] = 0;
                nIndices[j] = 0;
            }

            currentFaceIndex = 0;
            currentIndex = 0;
            slashCounter = 0;


            f++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST); // デプスバッファを使用 | glutInitDisplayMode() で GLUT_DEPTH を指定する

    // 光源の設定
    GLfloat lightPosition0[] = { -50.0, -50.0, 20.0, 1.0 }; // 光源0の座標
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void drawGround(void) {
    double groundMaxX = 500.0;
    double groundMaxY = 500.0;

    glColor3d(0.8, 0.8, 0.8); // グリッド線の色

    glBegin(GL_LINES);

    for (double ly = -groundMaxY; ly <= groundMaxY; ly += 10.0) {
        glVertex3d(-groundMaxX, ly, 0);
        glVertex3d(groundMaxX, ly, 0);
    }

    for (double lx = -groundMaxX; lx <= groundMaxX; lx += 10.0) {
        glVertex3d(lx, groundMaxY, 0);
        glVertex3d(lx, -groundMaxY, 0);
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double viewX = 0.0;
double viewY = -200.0;
double viewZ = 20.0;

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // バッファの消去


    //////////////////////////////////////////////////
    // 物質質感の定義
    //////////////////////////////////////////////////

    struct MaterialStruct {
        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat shininess;
    };

    // jade (ヒスイ)
    MaterialStruct ms_jade = {
      { 0.135,     0.2225,   0.1575,   1.0 },
      { 0.54,      0.89,     0.63,     1.0 },
      { 0.316228,  0.316228, 0.316228, 1.0 },
      12.8
    };

    // ruby (ルビー)
    MaterialStruct ms_ruby = {
      { 0.1745,   0.01175,  0.01175,   1.0 },
      { 0.61424,  0.04136,  0.04136,   1.0 },
      { 0.727811, 0.626959, 0.626959,  1.0 },
      76.8
    };


    //////////////////////////////////////////////////
    // 色の定義
    //////////////////////////////////////////////////

    GLfloat red[]    = { 0.8, 0.2, 0.2, 1.0 }; // 赤色
    GLfloat green[]  = { 0.2, 0.8, 0.2, 1.0 }; // 緑色
    GLfloat blue[]   = { 0.2, 0.2, 0.8, 1.0 }; // 青色
    GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 }; // 黄色
    GLfloat white[]  = { 1.0, 1.0, 1.0, 1.0 }; // 白色

    GLfloat shininess = 30.0; // 光沢の強さ


    //////////////////////////////////////////////////
    // 視点の移動
    //////////////////////////////////////////////////

    viewY += (-50.0 - viewY) * 0.001;


    //////////////////////////////////////////////////
    // 透視変換行列の設定
    //////////////////////////////////////////////////

    glMatrixMode(GL_PROJECTION); // 行列モードの設定（GL_PROJECTION：透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
    glLoadIdentity();            // 行列の初期化
    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0); // 透視投影法の視体積 | gluPerspactive(th, w/h, near, far)


    //////////////////////////////////////////////////
    // 視点の設定
    //////////////////////////////////////////////////

    gluLookAt(
        0.0, viewY, viewZ,     // 視点の位置
        0.0, viewY + 200, 0.0, // 視界の中心位置の参照点座標
        0.0, 0.0, 1.0          // 視界の上方向のベクトル
    );


    //////////////////////////////////////////////////
    // モデルビュー変換行列の設定
    //////////////////////////////////////////////////

    glMatrixMode(GL_MODELVIEW); // 行列モードの設定（GL_PROJECTION：透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
    glLoadIdentity();           // 行列の初期化
    glViewport(0, 0, windowWidth, windowHeight);


    //////////////////////////////////////////////////
    // 陰影ON
    //////////////////////////////////////////////////

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // 光源0を使用


    //////////////////////////////////////////////////
    // objファイルのモデルを描画
    //////////////////////////////////////////////////

    glPushMatrix();

    glColor3d(0.0, 0.0, 0.0); // 色の設定
    glTranslated(0, 50, 0);   // 平行移動値の設定

    glBegin(GL_TRIANGLES);

    for (int i = 0; i < COUNTOF(face); i++) {
        for (int j = 0; j < 9; j++) {
                 if (j % 3 == 2) glNormal3f(normal[face[i][j]][0], normal[face[i][j]][1], normal[face[i][j]][2]);
            else if (j % 3 == 1) glTexCoord2f(uv[face[i][j]][0], uv[face[i][j]][1]);
            else if (j % 3 == 0) glVertex3dv(vertex[face[i][j]]);
        }
    }

    glPopMatrix();

    glEnd();


    //////////////////////////////////////////////////
    // 陰影OFF
    //////////////////////////////////////////////////

    glDisable(GL_LIGHTING);


    //////////////////////////////////////////////////
    // 地面（グリッドの地平線）
    //////////////////////////////////////////////////

    drawGround();


    glutSwapBuffers(); // glutInitDisplayMode(GLUT_DOUBLE) でダブルバッファリングを利用
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void idle() {
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(windowPositionX, windowPositionY);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow(windowTitle);

    glutDisplayFunc(display); // 描画時に呼び出される関数
    glutIdleFunc(idle);       // アイドル時に呼び出される関数

    parseObjFileData(getFileContents("./assets/model.obj")); // objファイルからモデルデータをロード

    initialize();

    glutMainLoop();

    return 0;
}