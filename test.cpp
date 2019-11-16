inline int pred(int x) {
    if (x == 0)
        return 0;
    else
        return x - 1;
}

int main() {
  return pred(30) + pred(30) + pred(30);
}