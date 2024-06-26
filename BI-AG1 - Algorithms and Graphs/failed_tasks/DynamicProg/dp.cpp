#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>

using Price = unsigned long long;
using Employee = size_t;
inline constexpr Employee NO_EMPLOYEE = -1;
using Gift = size_t;

#endif

Price get_price(Price g, size_t e, size_t f, const std::vector<Price>& gifts){
  if(g==e || g==f){
    return ULLONG_MAX;
  }
  Price a = gifts[g]+gifts[e]+gifts[f];

  return gifts[g]+gifts[e]+gifts[f];
  
}

std::pair<Price, std::vector<Gift>> optimize_gifts(
  const std::vector<Employee>& boss,
  const std::vector<Price>& gift_price
) {
  std::vector<Price> gifts(gift_price);
  std::sort(gifts.begin(), gifts.end());
  //[boss] = employees
  std::vector<std::vector<Employee>> employees(boss.size(),std::vector<Employee>());

  for(size_t boss_id = 0; boss_id < boss.size(); boss_id++)
  {
    if(boss[boss_id]!=NO_EMPLOYEE)
      employees[boss[boss_id]].emplace_back(boss_id);
  }

  //TOPSORT
  std::queue<Employee> next;
  // indegrees and visited
  std::vector<std::pair<size_t , bool>> vertex_edges(boss.size(), std::make_pair(0, false));
  //[person] = pair of 2 best prices- price is pair of Gift index and *cena podstromu+[gift index]*
  std::vector<std::pair<std::pair<Price,Price>,std::pair<Price,Price>>> got_gift(boss.size(),std::make_pair(std::make_pair(ULLONG_MAX,ULLONG_MAX),std::make_pair(ULLONG_MAX,ULLONG_MAX)));
  for(Employee u: boss)
    if(u!=NO_EMPLOYEE)
		  vertex_edges[u].first++;

  for(size_t boss_id = 0; boss_id < boss.size(); boss_id++)
  {
	  if(vertex_edges[boss_id].first == 0)
	  {
		  next.emplace(boss_id);
		  vertex_edges[boss_id].second = true;
	  }
  }

  while(!next.empty())
  {
	  Employee elem = next.front();
	  next.pop();

    Employee boss_of_elem = boss[elem];
    if(boss_of_elem!=NO_EMPLOYEE){
      vertex_edges[boss_of_elem].first--;
      if(vertex_edges[boss_of_elem].first == 0)
      {
        next.emplace(boss_of_elem);
        vertex_edges[boss_of_elem].second = true;
      }
    }
    //when vertex is leaf, take 2 best gift options
    if(employees[elem].empty()){
      got_gift[elem] = std::make_pair(std::make_pair(0,gifts[0]),std::make_pair(1,gifts[1]));
    }
    else{
      size_t gift_idx = 0;
      for(const auto& g: gifts){
        Price min1 = 0,min2 = 0, min2idx = 0, min2div = ULLONG_MAX, empl_idx = 0;
        for(const auto& e: employees[elem]){
          if(got_gift[e].first.first == gift_idx && got_gift[e].second.first != gift_idx ){
              min1+=got_gift[e].second.second;
          }
          else if(got_gift[e].second.first == gift_idx && got_gift[e].first.first != gift_idx ){
              min1+=got_gift[e].first.second;
          }
          else{
            Price a= got_gift[e].second.second-got_gift[e].first.second;
            if(a < min2div){min2div = a; min2idx = empl_idx;}
            if(got_gift[e].second.second > got_gift[e].first.second)
              min1+=got_gift[e].first.second;
            else
              min1+=got_gift[e].second.second;
          }
          // for(const auto& f: employees[elem]){
          //   if(e==f) continue;
          //   Price one = get_price(gift_idx, got_gift[e].first.first, got_gift[f].first.first, gifts);
          //   //got_gift[e].first.first + got_gift[f].first.first; //1s1
          //   Price two = get_price(gift_idx, got_gift[e].first.first, got_gift[f].second.first, gifts);
          //   //got_gift[e].first.first + got_gift[f].second.first;//1s2
          //   Price three = get_price(gift_idx, got_gift[e].second.first, got_gift[f].first.first, gifts);
          //   //got_gift[e].second.first + got_gift[f].first.first;//2s1
          //   Price four = get_price(gift_idx, got_gift[e].second.first, got_gift[f].second.first, gifts);
          //   //got_gift[e].second.first + got_gift[f].second.first;//2s2
          //   Price min1 = one,min2 = two;
          //   if(min1 > three) min1 = three;
          //   else if(min2 > three) min2 = three;
          //   if(min1> four) min1 = four;
          //   else if(min2>four) min2 = four;
          //   if(min1>min2) std::swap(min1,min2);
          //   if(got_gift[elem].first.second > min1){
          //     got_gift[elem].second.second = got_gift[elem].first.second;
          //     got_gift[elem].second.first = got_gift[elem].first.first;
          //     got_gift[elem].first.second = min1;
          //     got_gift[elem].first.first = gift_idx;
          //     if(got_gift[elem].second.second > min2 && min1!=min2){
          //       got_gift[elem].second.second = min2;
          //       got_gift[elem].second.first = gift_idx;
          //     }
          //   }
          //   else if(got_gift[elem].second.second > min1 && got_gift[elem].first.second != min1){
          //       got_gift[elem].second.second = min1;
          //       got_gift[elem].second.first = gift_idx;
          //   }
          // }
          empl_idx++;
        }
        if(min2div!=ULLONG_MAX){
          empl_idx = 0;
          for(const auto& e: employees[elem]){
            if(got_gift[e].first.first == gift_idx && got_gift[e].second.first != gift_idx ){
              min2+=got_gift[e].second.second;
            }
            else if(got_gift[e].second.first == gift_idx && got_gift[e].first.first != gift_idx ){
                min2+=got_gift[e].first.second;
            }
            else{
              if(empl_idx == min2idx){
                if(got_gift[e].second.second > got_gift[e].first.second)
                  min2+=got_gift[e].second.second;
                else
                  min2+=got_gift[e].first.second;
              }
              else if(got_gift[e].second.second > got_gift[e].first.second)
                min2+=got_gift[e].first.second;
              else
                min2+=got_gift[e].second.second;
            }
            empl_idx++;
          }
        }
        if(got_gift[elem].first.second > min1+g){
          got_gift[elem].second.second = got_gift[elem].first.second;
          got_gift[elem].second.first = got_gift[elem].first.first;
          got_gift[elem].first.second = min1+g;
          got_gift[elem].first.first = gift_idx;
        if(got_gift[elem].second.second > min2+g && min1!=min2 && min2 != 0){
          got_gift[elem].second.second = min2+g;
          got_gift[elem].second.first = gift_idx;
        }
        }
        else if(got_gift[elem].second.second > min1+g && got_gift[elem].first.second != min1+g){
            got_gift[elem].second.second = min1+g;
            got_gift[elem].second.first = gift_idx;
        }
        gift_idx++;
      }

    }
	  
  }
}



#ifndef __PROGTEST__

const std::tuple<Price, std::vector<Employee>, std::vector<Price>> EXAMPLES[] = {
  // { 17, {1,2,3,4,NO_EMPLOYEE}, {25,4,18,3} },
  // { 16, {4,4,4,4,NO_EMPLOYEE}, {25,4,18,3} },
  // { 17, {4,4,3,4,NO_EMPLOYEE}, {25,4,18,3} },
  { 24, {4,4,3,4,NO_EMPLOYEE,3,3}, {25,4,18,3} },
};

#define CHECK(cond, ...) do { \
    if (cond) break; \
    printf("Test failed: " __VA_ARGS__); \
    printf("\n"); \
    return false; \
  } while (0)

bool test(Price p, const std::vector<Employee>& boss, const std::vector<Price>& gp) {
  auto&& [ sol_p, sol_g ] = optimize_gifts(boss, gp);
  CHECK(sol_g.size() == boss.size(),
    "Size of the solution: expected %zu but got %zu.", boss.size(), sol_g.size());

  Price real_p = 0;
  for (Gift g : sol_g) real_p += gp[g];
  CHECK(real_p == sol_p, "Sum of gift prices is %llu but reported price is %llu.", real_p, sol_p);

  if (0) {
    for (Employee e = 0; e < boss.size(); e++) printf(" (%zu)%zu", e, sol_g[e]);
    printf("\n");
  }

  for (Employee e = 0; e < boss.size(); e++)
    CHECK(boss[e] == NO_EMPLOYEE || sol_g[boss[e]] != sol_g[e],
      "Employee %zu and their boss %zu has same gift %zu.", e, boss[e], sol_g[e]);

  CHECK(p == sol_p, "Wrong price: expected %llu got %llu.", p, sol_p);

  return true;
}
#undef CHECK

int main() {
  int ok = 0, fail = 0;
  for (auto&& [ p, b, gp ] : EXAMPLES) (test(p, b, gp) ? ok : fail)++;
  
  if (!fail) printf("Passed all %d tests!\n", ok);
  else printf("Failed %d of %d tests.", fail, fail + ok);
}

#endif


