#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>

int windowPositionX = 100;
int windowPositionY = 100;

int windowWidth     = 512;
int windowHeight    = 512;

char windowTitle[] = "地平線";

void initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST); // デプスバッファを使用 | glutInitDisplayMode() で GLUT_DEPTH を指定する

    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0); // 透視投影法の視体積 | gluPerspactive(th, w/h, near, far)

    gluLookAt(
        0.0, -100.0, 25.0, // 視点の位置
        0.0, 100.0, 0.0,   // 視界の中心位置の参照点座標
        0.0, 0.0, 1.0      // 視界の上方向のベクトル
    );
};

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

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // バッファの消去


    //////////////////////////////////////////////////
    // 地面（地平線）
    //////////////////////////////////////////////////

    ground();


    glutSwapBuffers(); // glutInitDisplayMode(GLUT_DOUBLE) でダブルバッファリングを利用
};

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