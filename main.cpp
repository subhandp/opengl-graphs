#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <fstream>
#include <queue>
#include <stack>
#include <algorithm>
#include <vector>
#include <limits.h>
#include <math.h>
#include <sstream>
#include <map>
#include "graphx.h"
using namespace std;


static graph *prosesGraph;
static int mst, mstMenu, mstBobot, mstBobotMenu, findPath, findPathMenu, coloring, graphManipulate, mainx, mstSub, statAlgo = 0;
float xMidle, yMidle, jarak;
bool bobot = false, noBobot = false, pasangEdege = false;

vector< pair<float, float> > vertex; pair<bool, bool> findPathProp(false, false);
vector< vector<float> > adjacencyMatrix(100, vector <float>(100, -1));
vector< vector<float> > adjacencyTransversal; vector<bool> visitedVertex; vector <int> visitedVertexLog;
stringstream convert; string label; pair <bool, int> selectEdge(false, NULL);

void init(); void MidPoint(float xFrom, float yFrom, float xTujuan, float yTujuan);
void drawGraph(bool printAlgo); void display(); void djikstra(int source, int dest);

enum statusAlgo {mstAlgo = 1, mstBobotAlgo, findPathAlgo}; statusAlgo algoStatus;
enum colorEnum {hijau, merah, kuning, orange, hitam, putih};
enum algoritmaEnum {bfsEnum, dfsEnum, primEnum, djikstraEnum, backtrackingEnum};

int xMAx = 900, yMax = 700, kotaLabel = 1, corDiv = 15;


  void warna(colorEnum color){
    switch(color){
        case hijau:
            glColor3f(0.196, 0.804, 0.196);
            break;
        case merah:
            glColor3f(1, 0, 0);
            break;
        case kuning:
            glColor3f(1.000, 0.843, 0.000);
            break;
        case orange:
            glColor3f(1.000, 0.549, 0.000);
            break;
        case hitam:
            glColor3f(0, 0, 0);
            break;
        case putih:
            glColor3f(1, 1, 1);
            break;
    }
  }

  void drawString(float x,float y,float z, string text, int font = 18)
  {
    char *c;
    warna(putih);
    glRasterPos3f(x,y,z);
    for(int i = 0; i < text.length(); i++)
    {
      if (font == 12)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text.data()[i]);
      else if (font == 18)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text.data()[i]);
      else if (font == 24)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text.data()[i]);
    }
  }

void judulAwal(){
    glClear( GL_COLOR_BUFFER_BIT);
    switch (algoStatus){
        case mstAlgo:
            drawString(340.0,650.0,0.0, "MINIMUM SPANNING TREE");
            break;
        case mstBobotAlgo:
            drawString(290.0,650.0,0.0, "MINIMUM SPANNING TREE - BERBOBOT");
            break;
        case findPathAlgo:
            drawString(280.0,650.0,0.0, "FINDPATH / PENCARIAN JALUR TERPENDEK");
            break;
    }

}

  void drawKotak(float x, float y, string jenis = "normal", bool prosesAlg = false){
    if (selectEdge.first && !prosesAlg){
        float xSelect = vertex.at(selectEdge.second).first, ySelect = vertex.at(selectEdge.second).second;
        if (x == xSelect && y == ySelect ){
              warna(kuning);
              glPointSize(42.0);
              glBegin(GL_POINTS);
                  glVertex2f( x, y );
              glEnd();
          }
    }

    if (prosesAlg){
          warna(kuning);
          glPointSize(42.0);
          glBegin(GL_POINTS);
              glVertex2f( x, y);
          glEnd();
    }

    if (jenis == "normal")
      warna(orange);
    else if( jenis == "src")
      warna(merah);
    else if(jenis == "dest")
      warna(hijau);

    glPointSize(30.0);
    glBegin(GL_POINTS);
        glVertex2f(x,y);
    glEnd();

    convert << kotaLabel;
    label = convert.str();
    drawString(x-7,y-5,0.0,label);
    kotaLabel++;
    convert.str("");
  }

     void drawEdge(int i, int j,bool prosesAlg = false){
        if (i == -1 || j == -1)
            return;
        if(!prosesAlg)
            glLineWidth(1), warna(putih);
        else
            glLineWidth(5), warna(merah);

        glBegin(GL_LINES);
            glVertex2f( vertex.at(i).first , vertex.at(i).second );
            glVertex2f( vertex.at(j).first , vertex.at(j).second );
        glEnd();

        if (algoStatus != mstAlgo){
            float jarak = prosesGraph->getData().adjacencyMatrix.at(i).at(j);
            convert << jarak;
            label = convert.str();
            convert.str("");
            MidPoint(vertex.at(i).first , vertex.at(i).second, vertex.at(j).first, vertex.at(j).second);
            drawString(xMidle, yMidle,0, label, 12);

        }

     }



void MidPoint(float xFrom, float yFrom, float xTujuan, float yTujuan){
  xMidle = (xFrom + xTujuan) / 2;
  yMidle = (yFrom + yTujuan) / 2;
}

bool selectEdgeSame(float x, float y){
   for (int i = 0; i < vertex.size() ; i++)
    {
        if ( (x >= vertex.at(i).first && x <= (vertex.at(i).first+15)) || (x <= vertex.at(i).first && x >= (vertex.at(i).first-15)) )
            if ( (y >= vertex.at(i).second && y <= (vertex.at(i).second+15)) || (y <= vertex.at(i).second && y >= (vertex.at(i).second-15)) ){
                if(!selectEdge.first){
                    selectEdge.second = i;
                    selectEdge.first = true;
                    drawGraph(false);
                }
                else
                    if (selectEdge.second == i){
                        selectEdge.second = NULL, selectEdge.first = false;
                        drawGraph(false);
                    }
                    else{
                        float bobot;
                        float jarak = sqrt( pow( fabs( (vertex.at(selectEdge.second).first - vertex.at(i).first) ), 2 ) + pow( fabs( (vertex.at(i).second - vertex.at(selectEdge.second).second ) ), 2) );
                        bobot = algoStatus == mstAlgo ? 1 : jarak, bobot =  algoStatus == mstAlgo ? 1 : jarak;
                        prosesGraph->addEdge(selectEdge.second, i, bobot);
                        drawGraph(false);
                    }

                return true;
            }

    }

    return false;
}

void setPoint(float x, float y){
  if(algoStatus){
    if (selectEdgeSame(x, y))
        return;

    vertex.push_back(make_pair(x,y));
    drawGraph(false);

  }else{
    cout << "PILIH METODE ALGORITMA DULU" << endl;
  }


}



void drawGraph(bool printAlgo = false){
    judulAwal();
    float nil; kotaLabel = 1; string node = "normal";
    for (int i = 0; i < vertex.size(); i++){
        if (algoStatus == findPathAlgo){
            if (i == 0)
                node = "src";
            else if (i == 1)
                node = "dest";
            else
                node = "normal";
        }
        drawKotak(vertex.at(i).first, vertex.at(i).second, node);
    }

    for (int v = 0; v < vertex.size(); v++){
        for (int j = 0; j < vertex.size(); j++){
            nil = printAlgo ? prosesGraph->getData().adjacencyTransversal.at(v).at(j) : prosesGraph->getData().adjacencyMatrix.at(v).at(j);
            if (nil != -1)
              drawEdge(v, j);
        }
    }

}

void drawAlgo(){
    string node = "normal";
    cout << "size: " << prosesGraph->getData().visitedVertexLog.size() << endl;

    for (int i = 0; i < prosesGraph->getData().visitedVertexLog.size(); i++){
        int vertexLog = prosesGraph->getData().visitedVertexLog.at(i);
        cout << "i+1" << i+1 << endl;
        int vertexLogNext = i+1 >= prosesGraph->getData().visitedVertexLog.size() ? -1 : prosesGraph->getData().visitedVertexLog.at(i+1);

        if (algoStatus == findPathAlgo){
            if (vertexLog == 0)
                node = "src";
            else if (vertexLog == 1)
                node = "dest";
            else
                node = "normal";
        }
        kotaLabel = vertexLog + 1;
        for (int i = 0; i <= 100000; i++){cout << i << endl;}
        drawKotak(vertex.at(vertexLog).first, vertex.at(vertexLog).second, node, true);
        glFlush();
    }

    if (algoStatus == findPathAlgo)
                for (int i = 0; i < prosesGraph->getData().shortest.size() - 1; i++ )
                    drawEdge(prosesGraph->getData().shortest.at(i), prosesGraph->getData().shortest.at(i+1), true );


}



void allNodeConnected(){
    if (vertex.size() < 2){
        cout << "Jumlah vertex minimal 2!" << endl;
        return;
    }

    for (int i = 0; i < vertex.size() ; i++)
      {
        for (int j = 0; j < vertex.size() ; j++){
            if(i != j){
                float bobot;
                float jarak = sqrt( pow( fabs( (vertex.at(i).first - vertex.at(j).first) ), 2 ) + pow( fabs( (vertex.at(i).second - vertex.at(j).second) ), 2) );
                bobot =  algoStatus == mstAlgo ? 1 : jarak, bobot =  algoStatus == mstAlgo ? 1 : jarak;
                prosesGraph->addEdge(i, j, bobot);

            }
        }

      }

     if (algoStatus == findPathAlgo){
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < vertex.size(); j++)
                prosesGraph->removeEdge(i, j);

    }

    drawGraph(false);
}

void allNodeNotConnected(){
    if (vertex.size() < 2){
        cout << "Jumlah vertex minimal 2 !" << endl;
        return;
    }

    for (int i = 0; i < vertex.size() ; i++)
        for (int j = 0; j < vertex.size() ; j++)
                prosesGraph->removeEdge(i, j);

    drawGraph(false);
}

void display(){
    if (!algoStatus){
            drawString(380.0,650.0,0.0,"GRAPH ALGORITHM", 24);
        drawString(100.0,550.0,0.0,"[F1] POHON RENTANG MINIMUM", 24);
            drawString(150.0,520.0,0.0,"1. Depth First Search (DFS) ", 18);
            drawString(150.0,490.0,0.0,"2. Breadth First Search (BFS) ", 18);

        drawString(100.0,410.0,0.0,"[F2] POHON RENTANG MINIMUM - BERBOBOT", 24);
            drawString(150.0,380.0,0.0,"1. Prim ", 18);
            drawString(150.0,350.0,0.0,"2. Kruskal ", 18);

        drawString(100.0,270.0,0.0,"[F3] PATHFINDING / JALUR TERPENDEK", 24);
            drawString(150.0,240.0,0.0,"1. Djikstra ", 18);
            drawString(150.0,210.0,0.0,"2. A* (Star) ", 18);

        drawString(100.0,140.0,0.0,"[UTILITY] COLORING GRAPH", 24);
            drawString(150.0,110.0,0.0,"1. Backtracking", 18);
    }

    glFlush();
}



void mouse(int mouse, int state, int x, int y){
    switch(mouse){
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN){
                float a = x/1.0;
                float b = yMax - (y/1.0);
                setPoint(a,b);
                glFlush();
            }
        break;

    }

}




void ProcessMenu(int value){
    cout << "prosess menu";
    if (value == 7)
        allNodeConnected();
    else if (value == 8)
        allNodeNotConnected();
  }


void algoritma(int algo){
    switch(algo){
        case bfsEnum:
            prosesGraph->proses("bfs", 0, false, vertex.size());
            break;
        case dfsEnum:
            prosesGraph->proses("dfs", 0, false, vertex.size());
            break;
        case primEnum:
            prosesGraph->proses("prim", 0, false, vertex.size());
            break;
        case djikstraEnum:
            prosesGraph->proses("djikstra", 0, 1, vertex.size());
            break;
    }
    drawAlgo();
}


void createMenu(void){

  mst = glutCreateMenu(algoritma);
  glutAddMenuEntry("BFS", bfsEnum);
  glutAddMenuEntry("DFS", dfsEnum);

  mstBobot = glutCreateMenu(algoritma);
  glutAddMenuEntry("Prim", primEnum);

  findPath = glutCreateMenu(algoritma);
  glutAddMenuEntry("Djikstra", djikstraEnum);

  coloring = glutCreateMenu(algoritma);
  glutAddMenuEntry("Node - (Backtracking)", backtrackingEnum);

  graphManipulate = glutCreateMenu(ProcessMenu);
  glutAddMenuEntry("Connect All Nodes", 7);
  glutAddMenuEntry("Disconnect All Nodes", 8);
  glutAddMenuEntry("Restore Graph", 9);
  glutAddMenuEntry("Delete Graph", 10);

  mstMenu = glutCreateMenu(ProcessMenu);
  glutAddSubMenu("Proses Algoritma", mst);
  glutAddMenuEntry("Skip Process", 1);
  glutAddSubMenu("Edit Graph", graphManipulate);
  glutAddSubMenu("Coloring Graph", coloring);
  glutAddMenuEntry("Back to Home.", 11);


  findPathMenu = glutCreateMenu(ProcessMenu);
  glutAddSubMenu("Proses Algoritma", findPath);
  glutAddMenuEntry("Skip Process", 1);
  glutAddSubMenu("Edit Graph", graphManipulate);
  glutAddSubMenu("Coloring Graph", coloring);
  glutAddMenuEntry("Back to Home.", 11);

  mstBobotMenu = glutCreateMenu(ProcessMenu);
  glutAddSubMenu("Proses Algoritma", mstBobot);
  glutAddMenuEntry("Skip Process", 1);
  glutAddSubMenu("Edit Graph", graphManipulate);
  glutAddSubMenu("Coloring Graph", coloring);
  glutAddMenuEntry("Back to Home.", 11);


  //glutAttachMenu(GLUT_RIGHT_BUTTON);


}

void special(int key, int, int)
{
    // handle special keys
    try {
        if (key == GLUT_KEY_F1 || key == GLUT_KEY_F2 || key == GLUT_KEY_F3)
            vertex.clear();
        switch (key)
            {
            case GLUT_KEY_F1:
                algoStatus = mstAlgo;
                glClear( GL_COLOR_BUFFER_BIT);
                prosesGraph =  graph::graphAlgoritma("mst");
                glutSetMenu(mstMenu);
                glutAttachMenu(GLUT_RIGHT_BUTTON);
                break;
             case GLUT_KEY_F2:
                algoStatus = mstBobotAlgo;
                glClear( GL_COLOR_BUFFER_BIT);
                prosesGraph =  graph::graphAlgoritma("mstBobot");
                glutSetMenu(mstBobotMenu);
                glutAttachMenu(GLUT_RIGHT_BUTTON);
                break;

             case GLUT_KEY_F3:
                algoStatus = findPathAlgo;
                glClear( GL_COLOR_BUFFER_BIT);
                prosesGraph =  graph::graphAlgoritma("findPath");
                glutSetMenu(findPathMenu);
                glutAttachMenu(GLUT_RIGHT_BUTTON);
                break;
            }
    }catch(int x){
        cout << "exception: " << x << endl;
    }

    judulAwal();
    glFlush();

    //glutPostRedisplay(); // force a redraw
}

void init(){
    glClearColor(0,0,0,0);
    glClear( GL_COLOR_BUFFER_BIT);
    gluOrtho2D(0.0, xMAx, 0.0, yMax);
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(xMAx,yMax);
    glutInitWindowPosition(200,100);
    glutCreateWindow("Graphs");
    createMenu();
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
   // glutPassiveMotionFunc(motionku);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
