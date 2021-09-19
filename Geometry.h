#ifndef Geometry_H
#define Geometry_H

struct Vertex
{
    float x, y, z;
    Vertex()
    {
        this->set(0, 0, 0);
    }

    Vertex(float x, float y, float z)
    {
        this->set(x, y, z);
    }

    void set(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

struct EdgePoint
{
    int x, y;
    boolean visible;

    EdgePoint()
    {
        this->set(0, 0);
        this->visible = false;
    }

    void set(int a, int b)
    {
        this->x = a;
        this->y = b;
    }
};

struct Point
{
    float x, y;

    Point()
    {
        set(0, 0);
    }

    Point(float x, float y)
    {
        set(x, y);
    }

    void set(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

};

struct squareFace
{
    int length;
    int sommets[4];
    int ed[4];

    squareFace()
    {
        set(-1, -1, -1, -1);
    }

    squareFace(int a, int b, int c, int d)
    {
        this->length = 4;
        this->sommets[0] = a;
        this->sommets[1] = b;
        this->sommets[2] = c;
        this->sommets[3] = d;
    }

    void set(int a, int b, int c, int d)
    {
        this->length = 4;
        this->sommets[0] = a;
        this->sommets[1] = b;
        this->sommets[2] = c;
        this->sommets[3] = d;
    }

};

struct triFace
{
  int length;
  int sommets[3];
  int ed[3];

  triFace()
  {
    set(-1,-1,-1);
  }
  triFace(int a, int b, int c)
  {
    this->length =3;
    this->sommets[0]=a;
    this->sommets[1]=b;
    this->sommets[2]=c;
  }
  void set(int a, int b, int c)
  { 
    this->length =3;
    this->sommets[0]=a;
    this->sommets[1]=b;
    this->sommets[2]=c;
  }
};

#endif