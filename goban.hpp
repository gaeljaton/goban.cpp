// -*- c++ -*-

#include <array>
#include <set>
#include <memory>
#include <numeric>
#include <iostream>
#include <cassert>

enum color_t {
  empty,
  black,
  white,
  out_of_board
};

class Point;
typedef std::shared_ptr<std::set<Point> > Points;
typedef std::shared_ptr<Point> Point_ptr;

class Point {
public:
  const int x, y;
  Point(const int, const int);

  Point up() const;
  Point down() const;
  Point left() const;
  Point right() const;

  Points around() const;

  bool operator<(const Point&) const;
  bool operator==(const Point&) const;
};

template<size_t N>
class Board {
private:
  std::array<std::array<color_t, N>, N> brd_state;
  Point_ptr ko_point;

  class get_chain_aux {
  private:
    const Board<N>& board;
    const color_t chain_color;
  public:
    get_chain_aux(const Board<N>& board, const color_t chain_color) :
      board(board),
      chain_color(chain_color) {}

    Points operator()(Points ps, const Point& p) const {
      if (board[p] != chain_color) {
        return ps;
      }
      else if (ps->find(p) != ps->end()) {
        return ps;
      }
      else {
        const Points aps = p.around();
        ps->insert(p);
        return accumulate(aps->begin(), aps->end(), ps, *this);
      }
    }
  };

public:
  Board() {
    for (unsigned int y = 0; y < N; y++) {
      for (unsigned int x = 0; x < N; x++) {
        brd_state[y][x] = empty;
      }
    }
  }

  Board(const Board<N>& other) :
    brd_state(other.brd_state),
    ko_point(other.ko_point) {}

  std::array<color_t, N>& operator[](const int y) {
    return brd_state[y];
  }

  color_t& operator[](const Point& p) {
    return brd_state[p.y][p.x];
  }

  color_t operator[](const Point& p) const {
    if (p.x < 0 ||
        p.y < 0 ||
        p.x >= static_cast<int>(N) ||
        p.y >= static_cast<int>(N)) {
      return out_of_board;
    }
    else {
      return brd_state[p.y][p.x];
    }
  }

  Points get_chain(const Point& p) const {
    Points ps(new std::set<Point>);
    const color_t c = (*this)[p];

    if (c == out_of_board || c == empty) {
      return ps;
    }
    else {
      get_chain_aux get_points_of_chain(*this, c);
      return get_points_of_chain(ps, p);
    }
  }

  bool alive_at(const Point& p) const {
    const Points ps = get_chain(p);
    for (std::set<Point>::const_iterator i = ps->begin();
         i != ps->end();
         i++) {
      const Points aps = i->around();
      for(std::set<Point>::const_iterator j = aps->begin();
          j != aps->end();
          j++) {
        if ((*this)[*j] == empty) return true;
      }
    }
    return false;
  }

  int put(const Point& p, const color_t c) {
    (*this)[p] = c;
    const Points aps = p.around();
    Points captured(new std::set<Point>);
    for (std::set<Point>::const_iterator i = aps->begin();
         i != aps->end();
         i++) {
      const color_t neighbor = (static_cast<const Board>(*this))[*i];
      if (neighbor != c &&
          neighbor != empty &&
          neighbor != out_of_board &&
          !alive_at(*i)) {
        const Points opponents = get_chain(*i);
        for (std::set<Point>::const_iterator j = opponents->begin();
             j != opponents->end();
             j++) {
          captured->insert(*j);
        }
      }
    }

    if (captured->size() == 1 &&
        get_chain(p)->size() == 1 &&
        !alive_at(p)) {
      ko_point = Point_ptr(new Point(*(captured->begin())));
    }
    else {
      ko_point = Point_ptr();
    }

    for (std::set<Point>::const_iterator i = captured->begin();
         i != captured->end();
         i++) {
      (*this)[*i] = empty;
    }

    return captured->size();
  }

  bool can_put(const Point& p, const color_t c) const {
    if ((*this)[p] != empty) {
      return false;
    }
    else if (ko_point && *ko_point == p) {
      return false;
    }
    else {
      Board b = (*this);
      b[p] = c;
      if (b.alive_at(p)) {
        return true;
      }
      else {
        const Points aps = p.around();
        for (std::set<Point>::const_iterator i = aps->begin();
             i != aps->end();
             i++) {
          const color_t neighbor = static_cast<const Board>(b)[*i];
          if (neighbor != c && neighbor != out_of_board && !b.alive_at(*i)) {
            return true;
          }
        }
        return false;
      }
    }
  }
};

template <size_t N>
std::ostream& operator<<(std::ostream& os, const Board<N> b) {
  for (size_t y = 0; y < N; y++) {
    for (size_t x = 0; x < N; x++) {
      switch (b[Point(x, y)]) {
      case empty:
        os << '.';
        break;
      case black:
        os << 'x';
        break;
      case white:
        os << 'o';
        break;
      case out_of_board:
        assert(false); // should never reach here
      }
    }
    os << std::endl;
  }
  return os;
}
