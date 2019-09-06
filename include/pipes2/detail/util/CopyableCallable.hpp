#pragma once

namespace tillh::pipes2
{
  template<class T>
  std::unique_ptr<T> deepCopy(const std::unique_ptr<T>& other)
  {
    return std::make_unique<T>(*other);
  }

  template<class F>
  class CopyableCallable
  {
  public:
    CopyableCallable(F f): f_(std::make_unique<F>(f)) {}

    CopyableCallable(const CopyableCallable& other):f_(deepCopy(other.f_)) {}
    CopyableCallable(CopyableCallable&&) = default;

    CopyableCallable& operator=(const CopyableCallable& other)
    {
      f_ = deepCopy(other.f_);
      return *this;
    }
    CopyableCallable& operator=(CopyableCallable&& other) = default;

    template<class... Ts>
    decltype(auto) operator()(Ts&& ... ts) const { return (*f_)(std::forward<Ts>(ts)...); }

  private:
    std::unique_ptr<F> f_;
  };
}
