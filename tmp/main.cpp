#include <iostream>
#include <vector>
#include <functional>
#include <string>

int main()
{
  // auto test = [&test](int x){
  //   test(2);
  // };

  auto add(auto a, auto b) -> decltype(add(1,2)) { return a + b; }

  test(1);
}
