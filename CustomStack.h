#ifndef CUSTOMSTACK_H_
#define CUSTOMSTACK_H_

template <typename T, int Capacity>
class CustomStack {
public:
  CustomStack();

  bool push(T item);
  T pop();
  bool isEmpty() const;
  bool isFull() const;

private:
  T buffer[Capacity];
  int top;
};

template <typename T, int Capacity>
CustomStack<T, Capacity>::CustomStack() : top(-1) {}

template <typename T, int Capacity>
bool CustomStack<T, Capacity>::push(T item) {
  if (isFull()) {
    return false;
  }

  buffer[++top] = item;
  return true;
}

template <typename T, int Capacity>
T CustomStack<T, Capacity>::pop() {
  if (isEmpty()) {
    return nullptr;
  }

  T item = buffer[top--];
  return item;
}

template <typename T, int Capacity>
bool CustomStack<T, Capacity>::isEmpty() const {
  return top == -1;
}

template <typename T, int Capacity>
bool CustomStack<T, Capacity>::isFull() const {
  return top == Capacity - 1;
}


#endif
