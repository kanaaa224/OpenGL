#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>

int windowPositionX = 100;
int windowPositionY = 100;

int windowWidth     = 512;
int windowHeight    = 512;

char windowTitle[] = "光と影";

////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
// 直方体の定義
//////////////////////////////////////////////////

// 頂点
GLdouble vertex[][3] = {
  { 0.0, 0.0, 0.0 },
  { 2.0, 0.0, 0.0 },
  { 2.0, 2.0, 0.0 },
  { 0.0, 2.0, 0.0 },
  { 0.0, 0.0, 30.0 },
  { 2.0, 0.0, 30.0 },
  { 2.0, 2.0, 30.0 },
  { 0.0, 2.0, 30.0 }
};

// 面の定義
int face[][4] = {
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 }
};

// 面の法線ベクトル
GLdouble normal[][3] = {
  { 0.0, 0.0,-1.0 },
  { 1.0, 0.0, 0.0 },
  { 0.0, 0.0, 1.0 },
  {-1.0, 0.0, 0.0 },
  { 0.0,-1.0, 0.0 },
  { 0.0, 1.0, 0.0 }
};


//////////////////////////////////////////////////
// 物質質感の定義
//////////////////////////////////////////////////

struct MaterialStruct {
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;
};

// jade (翡翠)
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

////////////////////////////////////////////////////////////////////////////////////////////////////

void initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST); // デプスバッファを使用 | glutInitDisplayMode() で GLUT_DEPTH を指定する

    //////////////////////////////////////////////////
    // 光源の設定
    //////////////////////////////////////////////////
    GLfloat light_position0[] = { -50.0, -50.0, 20.0, 1.0 }; // 光源0の座標
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0); // 透視投影法の視体積 | gluPerspactive(th, w/h, near, far);

    gluLookAt(
        0.0, -100.0, 50.0, // 視点の位置
        0.0, 100.0, 0.0,   // 視界の中心位置の参照点座標
        0.0, 0.0, 1.0      // 視界の上方向のベクトル
    );
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void ground(void) {
    double groundMaxX = 300.0;
    double groundMaxY = 300.0;

    glColor3d(0.8, 0.8, 0.8); // 大地の色

    glBegin(GL_LINES);

    for (double ly = -groundMaxY; ly <= groundMaxY; ly += 10.0) {
        glVertex3d(-groundMaxX, ly, 0);
        glVertex3d(groundMaxX, ly, 0);
    };

    for (double lx = -groundMaxX; lx <= groundMaxX; lx += 10.0) {
        glVertex3d(lx, groundMaxY, 0);
        glVertex3d(lx, -groundMaxY, 0);
    };

    glEnd();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // バッファの消去

    // 球
    glPushMatrix();
    glColor3d(1.0, 0.0, 0.0);      // 色の設定
    glTranslated(0.0, 10.0, 20.0); // 平行移動値の設定
    glutSolidSphere(4.0, 20, 20);  // 引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();

    // 立方体
    glPushMatrix();
    glColor3d(0.0, 1.0, 0.0);       // 色の設定
    glTranslated(-20.0, 0.0, 20.0); // 平行移動値の設定
    glutSolidCube(10.0);            // 引数：(一辺の長さ)
    glPopMatrix();

    // 円錐
    glPushMatrix();
    glColor3d(0.0, 0.0, 1.0);         // 色の設定
    glTranslated(20.0, 100.0, 0.0);   // 平行移動値の設定
    glutSolidCone(5.0, 10.0, 20, 20); // 引数：(半径, 高さ, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();

    // 直方体
    glPushMatrix();
    glColor3d(0.0, 1.0, 1.0);      // 色の設定
    glTranslated(30.0, 50.0, 0.0); // 平行移動値の設定
    glBegin(GL_QUADS);
    for (int j = 0; j < 6; ++j) {
        for (int i = 0; i < 4; ++i) {
            glVertex3dv(vertex[face[j][i]]);
        };
    };
    glEnd();
    glPopMatrix();

    ground(); // 地面

    glutSwapBuffers(); // glutInitDisplayMode(GLUT_DOUBLE)でダブルバッファリングを利用
};

////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowPosition(windowPositionX, windowPositionY);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

    glutCreateWindow(windowTitle);
    glutDisplayFunc(display); // 描画時に呼び出される関数を指定

    initialize();

    glutMainLoop();

    return 0;
};