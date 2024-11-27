#include <vector>
#include <GL/glut.h>

#define COUNTOF(array) (sizeof(array) / sizeof(array[0])) // 配列の要素数を取得
#define CHAR_TO_INT(c) ((c) - '0')

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

int windowWidth  = 1280;
int windowHeight = 720;

double viewX = 0.0;
double viewY = -200.0;
double viewZ = 20.0;

int state = 0;

struct Model {
    double  x,  y,  z; // 位置
    double vx, vy, vz; // 移動量
    double ix, iy, iz; // 初期位置
};

std::vector<Model> sphere_models;
std::vector<Model> cube_models;

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
    // 透視変換行列の設定
    //////////////////////////////////////////////////

    glMatrixMode(GL_PROJECTION); // 行列モードの設定（GL_PROJECTION: 透視変換行列の設定, GL_MODELVIEW: モデルビュー変換行列）
    glLoadIdentity();            // 行列の初期化
    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 1000.0); // 透視投影法の視体積


    //////////////////////////////////////////////////
    // 視点の設定
    //////////////////////////////////////////////////

    gluLookAt(
        viewX, viewY, viewZ, // 視点の位置
        0.0, 0.0, viewZ,     // 視界の中心位置の参照点座標
        0.0, 0.0, 1.0        // 視界の上方向のベクトル
    );


    //////////////////////////////////////////////////
    // モデルビュー変換行列の設定
    //////////////////////////////////////////////////

    glMatrixMode(GL_MODELVIEW); // 行列モードの設定（GL_PROJECTION: 透視変換行列の設定, GL_MODELVIEW: モデルビュー変換行列）
    glLoadIdentity();           // 行列の初期化
    glViewport(0, 0, windowWidth, windowHeight);


    //////////////////////////////////////////////////
    // 地面（グリッドの地平線）
    //////////////////////////////////////////////////

    drawGround();


    //////////////////////////////////////////////////
    // テキスト 描画
    //////////////////////////////////////////////////

    char t_char[100];
    char t_char2[100];

    strcpy_s(t_char2, "Number of sphere models: ");
    sprintf_s(t_char, "%d", sphere_models.size());
    strcat_s(t_char2, t_char);
    drawText(1, 95, t_char2);

    strcpy_s(t_char2, "Number of cube models: ");
    sprintf_s(t_char, "%d", cube_models.size());
    strcat_s(t_char2, t_char);
    drawText(1, 90, t_char2);


    //////////////////////////////////////////////////
    // 陰影 ON
    //////////////////////////////////////////////////

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // 光源0を使用


    //////////////////////////////////////////////////
    // 球のモデル 描画
    //////////////////////////////////////////////////

    for (int i = 0; i < sphere_models.size(); i++) {
        sphere_models[i].x += sphere_models[i].vx;
        sphere_models[i].y += sphere_models[i].vy;
        sphere_models[i].z += sphere_models[i].vz;

        glPushMatrix();

        glTranslated(sphere_models[i].x, sphere_models[i].y, sphere_models[i].z); // 平行移動値の設定

        glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);

        glutSolidSphere(4.0, 20, 20);

        glPopMatrix();
    }


    //////////////////////////////////////////////////
    // 球のモデル 描画
    //////////////////////////////////////////////////

    for (int i = 0; i < cube_models.size(); i++) {
        cube_models[i].x += cube_models[i].vx;
        cube_models[i].y += cube_models[i].vy;
        cube_models[i].z += cube_models[i].vz;

        glPushMatrix();

        glTranslated(cube_models[i].x, cube_models[i].y, cube_models[i].z); // 平行移動値の設定

        glMaterialfv(GL_FRONT, GL_AMBIENT, ms_jade.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_jade.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ms_jade.specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, &ms_jade.shininess);

        glutSolidCube(10);

        glPopMatrix();
    }


    //////////////////////////////////////////////////
    // 陰影 OFF
    //////////////////////////////////////////////////

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


    //////////////////////////////////////////////////
    // モデルの自動生成
    //////////////////////////////////////////////////

    if (state) {
        Model model;
        model.x = 0.0;
        model.y = 0.0;
        model.z = 0.0;
        model.vx = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.vy = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.vz = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.ix = model.x;
        model.iy = model.y;
        model.iz = model.z;
        sphere_models.push_back(model);

        model.x = 0.0;
        model.y = 0.0;
        model.z = 0.0;
        model.vx = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.vy = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.vz = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.ix = model.x;
        model.iy = model.y;
        model.iz = model.z;
        cube_models.push_back(model);
    }


    glutSwapBuffers(); // ダブルバッファリングを使用 | glutInitDisplayMode() で GLUT_DOUBLE を引数に設定
}

void idle() {
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        viewY++;
        break;

    case 's':
        viewY--;
        break;

    case 'a':
        viewX--;
        break;

    case 'd':
        viewX++;
        break;

    case 'q':
        viewZ++;
        break;

    case 'e':
        viewZ--;
        break;

    case 'x': {
        if (state) state = 0;
        else       state = 1;
        break;
    }

    case 'm': {
        Model model;
        model.x = 0.0;
        model.y = 0.0;
        model.z = 0.0;
        model.vx = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.vy = 0.0;
        model.vz = (((rand() % 10 + 1) + -5.0) / 10.0);
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
        model.vx = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.vy = 0.0;
        model.vz = (((rand() % 10 + 1) + -5.0) / 10.0);
        model.ix = model.x;
        model.iy = model.y;
        model.iz = model.z;
        cube_models.push_back(model);
        break;
    }

    case 'z':
        sphere_models.clear();
        cube_models.clear();
        break;

    case '0':
        exit(0);
        break;

    default:
        break;
    }
}

void initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST); // デプスバッファを使用 | glutInitDisplayMode() で GLUT_DEPTH を引数に設定

    // 光源の設定
    GLfloat lightPosition0[] = { -50.0, -50.0, 20.0, 1.0 }; // 光源0の座標
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL [ WASDQE: カメラ移動 / MN: モデルを生成 / X: モデルを自動生成 / Z: リセット ]");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    initialize();

    glutMainLoop();

    return 0;
}