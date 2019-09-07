

namespace tillh::pipes
{
  template<class Output>
  class OutputIteratorBase
  {
    struct Assignable
    {
      Assignable(Output& output):output_(output) {}

      template<class T>
      Assignable& operator=(T&& t)
      {
        output_.push(std::forward<T>(t));
        return *this;
      }

      Output& output_;
    };
  public:
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using value_type = void;
    using pointer = void;
    using reference = void;

    Output& operator++() { return *this_(); }
    Output operator++(int) { return *this_(); }

    Assignable operator*()
    {
      return Assignable(*this_());
    }

  protected:
    OutputIteratorBase() = default;

  private:
    Output* this_() { return static_cast<Output*>(this); }
  };
}
