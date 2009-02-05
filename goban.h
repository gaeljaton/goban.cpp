// -*- c++ -*-

#include <vector>

using namespace std;

enum color_t {
  empty,
  black,
  white
};

class Point {
public:
  int row, col;
  Point(const int, const int);
  Point up() const;
  Point down() const;
  Point left() const;
  Point right() const;

  bool operator<(const Point) const;
};

class Board {
private:
  int size;
  vector<vector<color_t> > brd_state;

public:
  Board(const int size);
  color_t ref(const Point) const;
  void set(const Point, const color_t);
};
