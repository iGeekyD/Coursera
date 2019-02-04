#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template <typename T> struct Cmp {
  bool operator()(const pair<T *, int> &lhs, const pair<T *, int> &rhs) const {
    return lhs.second > rhs.second;
  }
};

template <typename T> class PriorityCollection {
public:
  using Id = T * /* тип, используемый для идентификаторов */;
  using priorityId = typename set<pair<Id, int>, Cmp<T>>::const_iterator;
  PriorityCollection() { objects.reserve(1000000); }
  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
    objects.push_back(move(object));
    T *id = &objects.back();
    priority.insert(make_pair(id, 0));
    priorityId pId = priority.find(make_pair(id, 0));
    intermap[id] = pId;
    return id;
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
    for (auto it = range_begin; it != range_end; ++it) {
      ids_begin = Add(*it);
      ++ids_begin;
    }
  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const { return intermap.find(id) != intermap.end(); }

  // Получить объект по идентификатору
  const T &Get(Id id) const { return *id; }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
    priorityId pId = intermap[id];
    int oldPriority = (*pId).second;
    priority.erase(pId);
    pId = priority.insert(priority.begin(), make_pair(id, oldPriority + 1));
    intermap[id] = pId;
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T &, int> GetMax() const {
    auto tmpPair = *(priority.begin());
    return make_pair(*(tmpPair.first), tmpPair.second);
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
    auto tmpPair = *(priority.begin());
    Id id = tmpPair.first;
    priority.erase(priority.begin());
    intermap.erase(id);
    auto tmp = make_pair(move(*id), tmpPair.second);
    return tmp;
  }

private:
  // Приватные поля и методы
  set<pair<Id, int>, Cmp<T>> priority;
  map<Id, priorityId> intermap;
  vector<T> objects;
};

class StringNonCopyable : public string {
public:
  using string::string; // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable &) = delete;
  StringNonCopyable(StringNonCopyable &&) = default;
  StringNonCopyable &operator=(const StringNonCopyable &) = delete;
  StringNonCopyable &operator=(StringNonCopyable &&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  const auto white_id = strings.Add("white");
  const auto yellow_id = strings.Add("yellow");
  const auto red_id = strings.Add("red");

  strings.Promote(yellow_id);
  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "yellow");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "white");
    ASSERT_EQUAL(item.second, 0);
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}
