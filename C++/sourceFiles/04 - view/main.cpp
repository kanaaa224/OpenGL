#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>

int windowPositionX = 100;
int windowPositionY = 100;

int windowWidth  = 512;
int windowHeight = 512;

char windowTitle[] = "視点";

////////////////////////////////////////////////////////////////////////////////////////////////////

void initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST); // デプスバッファを使用 | glutInitDisplayMode() で GLUT_DEPTH を指定する

    // 光源の設定
    GLfloat lightPosition0[] = { -50.0, -50.0, 20.0, 1.0 }; // 光源0の座標
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void ground(void) {
    double groundMaxX = 500.0;
    double groundMaxY = 500.0;

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


    //////////////////////////////////////////////////
    // 視点の定義
    //////////////////////////////////////////////////

    double viewX = 0.0;
    double viewY = -200.0;
    double viewZ = 20.0;


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
    // 視点
    //////////////////////////////////////////////////

    viewY += (-50.0 - viewY) * 0.25;


    //////////////////////////////////////////////////
    // 透視変換行列の設定
    //////////////////////////////////////////////////

    glMatrixMode(GL_PROJECTION); // 行列モードの設定（GL_PROJECTION：透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
    glLoadIdentity();            // 行列の初期化
    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0); // 透視投影法の視体積 | gluPerspactive(th, w/h, near, far);


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
    // 球
    //////////////////////////////////////////////////

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);
    glTranslated(0.0, 10.0, 20.0); // 平行移動値の設定
    glutSolidSphere(4.0, 20, 20);  // 引数：半径、Z軸まわりの分割数、Z軸に沿った分割数
    glPopMatrix();


    //////////////////////////////////////////////////
    // 立方体
    //////////////////////////////////////////////////

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    glTranslated(-20.0, 0.0, 20.0); // 平行移動値の設定
    glutSolidCube(10.0);            // 引数：一辺の長さ
    glPopMatrix();


    //////////////////////////////////////////////////
    // 円錐
    //////////////////////////////////////////////////

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    glTranslated(20.0, 100.0, 0.0);   // 平行移動値の設定
    glutSolidCone(5.0, 10.0, 20, 20); // 引数：半径、高さ、Z軸まわりの分割数、Z軸に沿った分割数
    glPopMatrix();


    //////////////////////////////////////////////////
    // 直方体
    //////////////////////////////////////////////////

    // 頂点の定義
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

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, ms_jade.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_jade.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ms_jade.specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &ms_jade.shininess);
    glTranslated(30.0, 50.0, 0.0); // 平行移動値の設定
    glBegin(GL_QUADS);
    for (int j = 0; j < 6; ++j) {
        glNormal3dv(normal[j]); // 法線ベクトルの指定
        for (int i = 0; i < 4; ++i) {
            glVertex3dv(vertex[face[j][i]]);
        };
    };
    glPopMatrix();

    glEnd();


    //////////////////////////////////////////////////
    // 陰影OFF
    //////////////////////////////////////////////////

    glDisable(GL_LIGHTING);


    ground(); // 地面

    glutSwapBuffers(); // glutInitDisplayMode(GLUT_DOUBLE) でダブルバッファリングを利用
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void idle() {
    glutPostRedisplay(); // glutDisplayFunc() を1回実行
};

////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowPosition(windowPositionX, windowPositionY);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

    glutCreateWindow(windowTitle);
    glutDisplayFunc(display); // 描画時に呼び出される関数を指定
    glutIdleFunc(idle);       // アイドル時に呼び出される関数を指定

    initialize();

    glutMainLoop();

    return 0;
};