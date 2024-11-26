#include <GL/glut.h>

void drawGround(void) {
    double maxX = 300.0;
    double maxY = 300.0;

    glColor3d(0.8, 0.8, 0.8); // 大地の色

    glBegin(GL_LINES);

    for (double ly = -maxY; ly <= maxY; ly += 10.0) {
        glVertex3d(-maxX, ly, 0);
        glVertex3d( maxX, ly, 0);
    }

    for (double lx = -maxX; lx <= maxX; lx += 10.0) {
        glVertex3d(lx,  maxY, 0);
        glVertex3d(lx, -maxY, 0);
    }

    glEnd();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // バッファの消去


    //////////////////////////////////////////////////
    // 球
    //////////////////////////////////////////////////

    glPushMatrix();
    glColor3d(1.0, 0.0, 0.0);      // 色の設定
    glTranslated(0.0, 10.0, 20.0); // 平行移動値の設定
    glutSolidSphere(4.0, 20, 20);  // 半径、Z軸まわりの分割数、Z軸に沿った分割数
    glPopMatrix();


    //////////////////////////////////////////////////
    // 立方体
    //////////////////////////////////////////////////

    glPushMatrix();
    glColor3d(0.0, 1.0, 0.0);       // 色の設定
    glTranslated(-20.0, 0.0, 20.0); // 平行移動値の設定
    glutSolidCube(10.0);            // 一辺の長さ
    glPopMatrix();


    //////////////////////////////////////////////////
    // 円錐
    //////////////////////////////////////////////////

    glPushMatrix();
    glColor3d(0.0, 0.0, 1.0);         // 色の設定
    glTranslated(20.0, 100.0, 0.0);   // 平行移動値の設定
    glutSolidCone(5.0, 10.0, 20, 20); // 半径、高さ、Z軸まわりの分割数、Z軸に沿った分割数
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

    glPushMatrix();
    glColor3d(0.0, 1.0, 1.0);      // 色の設定
    glTranslated(30.0, 50.0, 0.0); // 平行移動値の設定
    glBegin(GL_QUADS);
    for (int j = 0; j < 6; ++j) {
        for (int i = 0; i < 4; ++i) {
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glPopMatrix();

    glEnd();


    //////////////////////////////////////////////////
    // 地面（地平線）
    //////////////////////////////////////////////////

    drawGround();


    glutSwapBuffers(); // ダブルバッファリングを使用 | glutInitDisplayMode() で GLUT_DOUBLE を引数に設定
}

int windowWidth  = 500;
int windowHeight = 500;

void initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST); // デプスバッファを使用 | glutInitDisplayMode() で GLUT_DEPTH を引数に設定

    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0); // 透視投影法の視体積

    gluLookAt(
        0.0, -100.0, 50.0, // 視点の位置
        0.0,  100.0,  0.0, // 視界の中心位置の参照点座標
        0.0,    0.0,  1.0  // 視界の上方向のベクトル
    );
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("OpenGL - 色んな物体");
    glutDisplayFunc(display);

    initialize();

    glutMainLoop();

    return 0;
}