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

    drawGround(); // 地面（地平線）

    glutSwapBuffers(); // ダブルバッファリングを使用 | glutInitDisplayMode() で GLUT_DOUBLE を引数に設定
}

int windowWidth  = 500;
int windowHeight = 500;

void initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST); // デプスバッファを使用 | glutInitDisplayMode() で GLUT_DEPTH を引数に設定

    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0); // 透視投影法の視体積

    gluLookAt(
        0.0, -100.0, 25.0, // 視点の位置
        0.0,  100.0,  0.0, // 視界の中心位置の参照点座標
        0.0,    0.0,  1.0  // 視界の上方向のベクトル
    );
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("OpenGL - 地平線");
    glutDisplayFunc(display);

    initialize();

    glutMainLoop();

    return 0;
}