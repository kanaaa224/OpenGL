#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <direct.h>
#include <GL/glut.h>
#include "gl_screenshot.h"
using namespace std;

double PI = acos(-1.0);

int WindowPositionX = 200;  //生成するウィンドウ位置のX座標
int WindowPositionY = 200;  //生成するウィンドウ位置のY座標
int WindowWidth = 512;  //生成するウィンドウの幅
int WindowHeight = 512;  //生成するウィンドウの高さ
char WindowTitle[] = "世界の始まり";  //ウィンドウのタイトル

double ViewPointX = 0.0;
double ViewPointY = -200.0;
double ViewPointZ = 20.0;

bool _Bitmap = true;
int tn = 0;
double t = 0;
double dt = 0.1;
double omega = 2.0 * PI / 10.0;
gl_screenshot gs; //bmpファイルの出力

//----------------------------------------------------
// 直方体の定義
//----------------------------------------------------
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
int face[][4] = {//面の定義
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 }
};
GLdouble normal[][3] = {//面の法線ベクトル
  { 0.0, 0.0,-1.0 },
  { 1.0, 0.0, 0.0 },
  { 0.0, 0.0, 1.0 },
  {-1.0, 0.0, 0.0 },
  { 0.0,-1.0, 0.0 },
  { 0.0, 1.0, 0.0 }
};
//----------------------------------------------------
// 物質質感の定義
//----------------------------------------------------
struct MaterialStruct {
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;
};
//jade(翡翠)
MaterialStruct ms_jade = {
  {0.135,     0.2225,   0.1575,   1.0},
  {0.54,      0.89,     0.63,     1.0},
  {0.316228,  0.316228, 0.316228, 1.0},
  12.8 };
//ruby(ルビー)
MaterialStruct ms_ruby = {
  {0.1745,   0.01175,  0.01175,   1.0},
  {0.61424,  0.04136,  0.04136,   1.0},
  {0.727811, 0.626959, 0.626959,  1.0},
  76.8 };
//----------------------------------------------------
// 色の定義の定義
//----------------------------------------------------
GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 }; //赤色
GLfloat green[] = { 0.2, 0.8, 0.2, 1.0 };//緑色
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };//青色
GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 };//黄色
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };//白色
GLfloat shininess = 30.0;//光沢の強さ
//-----------------------------------------

//----------------------------------------------------
// 関数プロトタイプ（後に呼び出す関数名と引数の宣言）
//----------------------------------------------------
void Initialize(void);
void Display(void);
void Idle();
void Ground(void);  //大地の描画Keyboard
//----------------------------------------------------
// メイン関数
//----------------------------------------------------
int main(int argc, char* argv[]) {
    if (_Bitmap) _mkdir("bitmap"); //bmpファイル保存用のフォルダの作成

    glutInit(&argc, argv);//環境の初期化
    glutInitWindowPosition(WindowPositionX, WindowPositionY);//ウィンドウの位置の指定
    glutInitWindowSize(WindowWidth, WindowHeight); //ウィンドウサイズの指定
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);//ディスプレイモードの指定
    glutCreateWindow(WindowTitle);  //ウィンドウの作成
    glutDisplayFunc(Display); //描画時に呼び出される関数を指定する（関数名：Display）

    glutIdleFunc(Idle);       //プログラムアイドル状態時に呼び出される関数
    Initialize(); //初期設定の関数を呼び出す
    glutMainLoop();
    return 0;
}
//----------------------------------------------------
// 初期設定の関数
//----------------------------------------------------
void Initialize(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0); //背景色
    glEnable(GL_DEPTH_TEST);//デプスバッファを使用：glutInitDisplayMode() で GLUT_DEPTH を指定する

    //光源の設定--------------------------------------
    GLfloat light_position0[] = { -50.0, -50.0, 20.0, 1.0 }; //光源0の座標
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0); //光源0を

}
//----------------------------------------------------
// 描画の関数
//----------------------------------------------------
void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //バッファの消去

    t = dt * tn;
    ViewPointX = -100.0 * cos(omega * t);
    ViewPointY = -100.0 * sin(omega * t);

    //透視変換行列の設定------------------------------
    glMatrixMode(GL_PROJECTION);//行列モードの設定（GL_PROJECTION : 透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
    glLoadIdentity();//行列の初期化
    gluPerspective(30.0, (double)WindowWidth / (double)WindowHeight, 0.1, 1000.0); //透視投影法の視体積gluPerspactive(th, w/h, near, far);

    //視点の設定------------------------------
    gluLookAt(
        ViewPointX, ViewPointY, ViewPointZ, // 視点の位置x,y,z;
        0.0, 0.0, ViewPointZ, // 視界の中心位置の参照点座標x,y,z
        0.0, 0.0, 1.0);     //視界の上方向のベクトルx,y,z
    //----------------------------------------

    //モデルビュー変換行列の設定--------------------------
    glMatrixMode(GL_MODELVIEW);//行列モードの設定（GL_PROJECTION : 透視変換行列の設定、GL_MODELVIEW：モデルビュー変換行列）
    glLoadIdentity();//行列の初期化
    glViewport(0, 0, WindowWidth, WindowHeight);
    //----------------------------------------------

    //陰影ON-----------------------------
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);//光源0を利用
    //-----------------------------------

    //球
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);
    glTranslated(0.0, 10.0, 20.0);//平行移動値の設定
    glutSolidSphere(4.0, 20, 20);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();

    //立方体
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    glTranslated(-20.0, -30.0, 20.0);//平行移動値の設定
    glutSolidCube(10.0);//引数：(一辺の長さ)
    glPopMatrix();

    //円錐
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    glTranslated(20.0, 110.0, 0.0);//平行移動値の設定
    glutSolidCone(5.0, 10.0, 20, 20);//引数：(半径, 高さ, Z軸まわりの分割数, Z軸に沿った分割数)
    glPopMatrix();

    //直方体
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, ms_jade.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_jade.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ms_jade.specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &ms_jade.shininess);
    glTranslated(30.0, 40.0, 0.0);//平行移動値の設定
    glBegin(GL_QUADS);
    for (int j = 0; j < 6; ++j) {
        glNormal3dv(normal[j]); //法線ベクトルの指定
        for (int i = 0; i < 4; ++i) {
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glEnd();
    glPopMatrix();

    //陰影OFF-----------------------------
    glDisable(GL_LIGHTING);
    //-----------------------------------

    Ground();

    if (_Bitmap) {
        ostringstream fname;
        int tt = tn + 10000;
        fname << "bitmap/" << tt << ".bmp";//出力ファイル名
        string name = fname.str();
        gs.screenshot(name.c_str(), 24);
    }
    tn++;

    glutSwapBuffers(); //glutInitDisplayMode(GLUT_DOUBLE)でダブルバッファリングを利用可
}
//----------------------------------------------------
// アイドル時に呼び出される関数
//----------------------------------------------------
void Idle() {
    glutPostRedisplay(); //glutDisplayFunc()を１回実行する
}

//----------------------------------------------------
// 大地の描画
//----------------------------------------------------
void Ground(void) {
    double ground_max_x = 500.0;
    double ground_max_y = 500.0;
    glColor3d(0.8, 0.8, 0.8);  // 大地の色
    glBegin(GL_LINES);
    for (double ly = -ground_max_y; ly <= ground_max_y; ly += 10.0) {
        glVertex3d(-ground_max_x, ly, 0);
        glVertex3d(ground_max_x, ly, 0);
    }
    for (double lx = -ground_max_x; lx <= ground_max_x; lx += 10.0) {
        glVertex3d(lx, ground_max_y, 0);
        glVertex3d(lx, -ground_max_y, 0);
    }
    glEnd();
}