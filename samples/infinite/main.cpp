// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <trieste/driver.h>
#include <trieste/token.h>
#include <trieste/wf.h>

using namespace trieste;
using namespace wf::ops;

inline constexpr auto X = TokenDef("x");
inline constexpr auto Y = TokenDef("y");
inline constexpr auto arg1 = TokenDef("arg1");

inline constexpr auto wfParser = wf::Wellformed(Top <<= (X | Y));
inline constexpr auto wfPass = wf::Wellformed(Top <<= (X | Y));

Parse parser()
{
  Parse p(depth::file);
  p.prefile([](auto&, auto& path) { return path.extension() == ".in"; });
  p.postfile([](auto&, auto&, auto) {});

  p("start",
    {
      "x" >> [](auto& m) { m.add(X); },
      "y" >> [](auto& m) { m.add(Y); },
    });

  return p;
}

PassDef infinite_growth()
{
  return {
    // Infinitely growing ASTs, overflows the stack.
    In(Group) * T(X)[arg1] >> [](Match& _) { return Group << _[arg1]; },
  };
}

PassDef equivalent_nodes()
{
  return {
    // Replace node with an equivalent one. Non-terminating, but doesn't grow.
    T(X)[arg1] >> [](Match& _) { return X << *_[arg1]; },
  };
}

PassDef see_saw()
{
  return {
    // Replace xs with ys and vice versa. Non-terminating, but doesn't grow.
    T(X)[arg1] >> [](Match& _) { return Y << *_[arg1]; },
    T(Y)[arg1] >> [](Match& _) { return X << *_[arg1]; },
  };
}

PassDef non_terminating_lambda()
{
  return {
    // Non-terminating lambda
    T(X)[arg1] >>
      [](Match& _) {
        while (true)
          ;
        return Y << *_[arg1];
      },
  };
}

PassDef recursive_lambda()
{
  std::function<TokenDef(Match&)> lambda = [&lambda](Match& _) -> TokenDef {
    return lambda(_);
  };
  return {
    // Recursive lambda, exhausts the stack.
    T(X)[arg1] >> lambda,
  };
}

Driver& driver()
{
  static Driver d(
    "demo",
    parser(),
    wfParser(),
    {
      {"infinite_growth", infinite_growth(), wfPass()},
      {"equivalent_nodes", equivalent_nodes(), wfPass()},
      {"see_saw", see_saw(), wfPass()},
      {"non_terminating_lambda", non_terminating_lambda(), wfPass()},
      {"recursive_lambda", recursive_lambda(), wfPass()},
    });

  return d;
}

int main(int argc, char** argv)
{
  return driver().run(argc, argv);
}