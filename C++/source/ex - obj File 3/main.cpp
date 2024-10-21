#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define DEGREE_RADIAN(_deg) (M_PI * (_deg) / 180.0f) // degree -> radian
#define RADIAN_DEGREE(_rad) ((_rad) * 180.0f / M_PI) // radian -> degree

#include <time.h>   // 日付時刻関連
#include <iostream> // 入出力関連
#include <fstream>  // ファイル操作関連
#include <string>   // 文字列操作関連
#include <map>      // 連想配列
#include <vector>   // 動的配列（要素を追加したり削除したり）
#include <sstream>  // 文字列ストリーム操作（文字列から数値への変換や数値から文字列への変換）

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
int windowWidth     = 1280;
int windowHeight    = 720;

char windowTitle[] = "objファイルからモデルをロードし斜方投射 [ WASD: 方向角度 / N: objモデルを発射 / M: 球を発射 / Z: リセット ]"; // TODO: "/ QE: 強さ"

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

void drawString(int x, int y, char* string, void* font) {
    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

int list;

void drawText(int x, int y, char* string) {
    glPushAttrib(GL_ENABLE_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(0.0, 0.0, 0.0);
    glCallList(list);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glMatrixMode(GL_MODELVIEW);
    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    drawString(x, y, string, GLUT_BITMAP_TIMES_ROMAN_24);
    glEndList();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void drawGround(void) {
    double groundMaxX = 500.0;
    double groundMaxY = 500.0;

    glColor3d(0.8, 0.8, 0.8);    // グリッド線の色
    glTranslated(0.0, 0.0, 0.0); // 平行移動値の設定

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

struct Model {
    double x, y, z;        // 位置
    double vx, vy, vz, vg; // 移動量、重力加速度（vector）
    double ix, iy, iz;     // 初期位置
};

std::vector<Model> obj_models;
std::vector<Model> sphere_models;
std::vector<Model> guide_models;

int launchAngleX   = 90; // 打ち出すX軸（横方向）のアングル
int launchAngleY   = 45; // 打ち出すY軸（縦方向）のアングル
int launchStrength = 0;  // 打ち出す強さ

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

    //viewY += (-50.0 - viewY) * 0.001;


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
        viewX, viewY, viewZ, // 視点の位置
        0.0, 0.0, 0.0,       // 視界の中心位置の参照点座標
        0.0, 0.0, 1.0        // 視界の上方向のベクトル
    );


    //////////////////////////////////////////////////
    // モデルビュー変換行列の設定
    //////////////////////////////////////////////////

    glMatrixMode(GL_MODELVIEW); // 行列モードの設定（GL_PROJECTION：透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
    glLoadIdentity();           // 行列の初期化
    glViewport(0, 0, windowWidth, windowHeight);


    //////////////////////////////////////////////////
    // 地面（グリッドの地平線）
    //////////////////////////////////////////////////

    drawGround();


    //////////////////////////////////////////////////
    // テキスト描画
    //////////////////////////////////////////////////

    char t_char[100];
    char t_char2[100];

    strcpy_s(t_char2, "Number of obj file models: ");
    sprintf_s(t_char, "%d", obj_models.size());
    strcat_s(t_char2, t_char);
    drawText(1, 95, t_char2);

    strcpy_s(t_char2, "Number of sphere models: ");
    sprintf_s(t_char, "%d", sphere_models.size());
    strcat_s(t_char2, t_char);
    drawText(1, 90, t_char2);

    strcpy_s(t_char2, "Launch Angle X: ");
    sprintf_s(t_char, "%d", launchAngleX);
    strcat_s(t_char2, t_char);
    drawText(1, 80, t_char2);

    strcpy_s(t_char2, "Launch Angle Y: ");
    sprintf_s(t_char, "%d", launchAngleY);
    strcat_s(t_char2, t_char);
    drawText(1, 75, t_char2);

    /*strcpy_s(t_char2, "Launch strength: ");
    sprintf_s(t_char, "%d", launchStrength);
    strcat_s(t_char2, t_char);
    drawText(1, 70, t_char2);*/


    //////////////////////////////////////////////////
    // 陰影ON
    //////////////////////////////////////////////////

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // 光源0を使用


    //////////////////////////////////////////////////
    // objファイルのモデルの描画、移動処理
    //////////////////////////////////////////////////

    for (int i = 0; i < obj_models.size(); i++) {
        obj_models[i].x += obj_models[i].vx;
        obj_models[i].y += obj_models[i].vy;
        obj_models[i].z += obj_models[i].vz;

        glPushMatrix();

        glTranslated(obj_models[i].x, obj_models[i].y, obj_models[i].z); // 移動値

        glMaterialfv(GL_FRONT, GL_AMBIENT, ms_jade.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_jade.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ms_jade.specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, &ms_jade.shininess);

        glBegin(GL_TRIANGLES);

        for (int f = 0; f < COUNTOF(face); f++) {
            for (int d = 0; d < 9; d++) {
                if (d % 3 == 2) glNormal3f(normal[face[f][d]][0], normal[face[f][d]][1], normal[face[f][d]][2]);
                else if (d % 3 == 1) glTexCoord2f(uv[face[f][d]][0], uv[face[f][d]][1]);
                else if (d % 3 == 0) glVertex3dv(vertex[face[f][d]]);
            }
        }

        glEnd();

        glPopMatrix();

        if (obj_models[i].vz < 0.0) obj_models[i].vz += -0.01;
        if (obj_models[i].vz > 0.0) obj_models[i].vz -= 0.01;

        if (obj_models[i].z < -0.1) {
            obj_models[i].vx = 0.0;
            obj_models[i].vy = 0.0;
            obj_models[i].vz = 0.0;
        }
    }


    //////////////////////////////////////////////////
    // ボールの描画、移動処理
    //////////////////////////////////////////////////

    for (int i = 0; i < sphere_models.size(); i++) {
        sphere_models[i].x += sphere_models[i].vx;
        sphere_models[i].y += sphere_models[i].vy;
        sphere_models[i].z += sphere_models[i].vz;

        glPushMatrix();

        glTranslated(sphere_models[i].x, sphere_models[i].y, sphere_models[i].z); // 移動値

        glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);

        glutSolidSphere(4.0, 20, 20); // 半径、Z軸まわりの分割数、Z軸に沿った分割数

        glPopMatrix();

        /*if (sphere_models[i].vx < 0.0) sphere_models[i].vx += -0.005;
        if (sphere_models[i].vx > 0.0) sphere_models[i].vx -= 0.005;
        if (sphere_models[i].vy < 0.0) sphere_models[i].vy += -0.005;
        if (sphere_models[i].vy > 0.0) sphere_models[i].vy -= 0.005;*/
        if (sphere_models[i].vz < 0.0) sphere_models[i].vz += -0.01;
        if (sphere_models[i].vz > 0.0) sphere_models[i].vz -= 0.01;

        if (sphere_models[i].z < -0.1) {
            sphere_models[i].vx = 0.0;
            sphere_models[i].vy = 0.0;
            sphere_models[i].vz = 0.0;
        }

        //for (int h = 0; h < sphere_models.size(); h++) {
        //    //
        //}
    }


    //////////////////////////////////////////////////
    // 陰影OFF
    //////////////////////////////////////////////////

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


    //////////////////////////////////////////////////
    // ガイドの描画
    //////////////////////////////////////////////////

    /*for (int i = 0; i < guide_models.size(); i++) {
        glPushMatrix();

        glTranslated(guide_models[i].x, guide_models[i].y, guide_models[i].z); // 移動値

        glColor3d(0.0, 0.0, 0.0);

        glutSolidSphere(1.0, 20, 20);

        glPopMatrix();
    }


    //////////////////////////////////////////////////
    // ガイドの生成
    //////////////////////////////////////////////////

    guide_models.clear();

    for (int i = 0; i < round(launchStrength); i++) {
        Model model;
        model.x = 0.0;
        model.y = 0.0;
        model.z = 0.0;
        guide_models.push_back(model);
    }*/


    glutSwapBuffers(); // glutInitDisplayMode(GLUT_DOUBLE) でダブルバッファリングを利用
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void idle() {
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        if (launchAngleY > 0) launchAngleY--;
        break;

    case 's':
        if (launchAngleY < 180) launchAngleY++;
        break;

    case 'd':
        if (launchAngleX > 0) launchAngleX--;
        break;

    case 'a':
        if (launchAngleX < 180) launchAngleX++;
        break;

    /*case 'q':
        launchStrength++;
        break;

    case 'e':
        launchStrength--;
        break;*/

    case 'z': {
        sphere_models.clear();
        obj_models.clear();
        break;
    }

    case 'm': {
        Model model;

        model.x = 0.0;
        model.y = 0.0;
        model.z = 0.0;

        model.vx = cosf(DEGREE_RADIAN(launchAngleX)) * cosf(DEGREE_RADIAN(launchAngleY));
        model.vy = sinf(DEGREE_RADIAN(launchAngleX)) * cosf(DEGREE_RADIAN(launchAngleY));
        model.vz = sinf(DEGREE_RADIAN(launchAngleY));
        model.vg = 0.0;

        model.ix = model.x;
        model.iy = model.y;
        model.iz = model.z;

        sphere_models.push_back(model);
        break;
    }

    case 'n': {
        Model model;

        model.x = 0.0;
        model.y = 0.0;
        model.z = 0.0;
        
        model.vx = cosf(DEGREE_RADIAN(launchAngleX)) * cosf(DEGREE_RADIAN(launchAngleY));
        model.vy = sinf(DEGREE_RADIAN(launchAngleX)) * cosf(DEGREE_RADIAN(launchAngleY));
        model.vz = sinf(DEGREE_RADIAN(launchAngleY));

        model.ix = model.x;
        model.iy = model.y;
        model.iz = model.z;

        obj_models.push_back(model);
        break;
    }

    case '0':
        exit(0);
        break;

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(windowPositionX, windowPositionY);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow(windowTitle);

    glutDisplayFunc(display);   // 描画時に呼び出される関数
    glutIdleFunc(idle);         // アイドル時に呼び出される関数
    glutKeyboardFunc(keyboard); // キーボード入力関数

    parseObjFileData(getFileContents("./assets/model.obj")); // objファイルからモデルデータをロード

    initialize();

    glutMainLoop();

    return 0;
}