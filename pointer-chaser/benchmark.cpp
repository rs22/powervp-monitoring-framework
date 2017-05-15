#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#define elements 15 * 1000 * 1000
#define times 100 * 1000 * 1000
#define NPAD 15

#define ONE data = data->n;
#define TEN ONE ONE ONE ONE ONE ONE ONE ONE ONE ONE
#define HUNDRED TEN TEN TEN TEN TEN TEN TEN TEN TEN TEN

typedef struct _test_t {
        struct _test_t *n;
        long int pad[NPAD];
    } test_t;

void fill_random(test_t * data)
{
  std::vector<size_t> list;
  for (size_t i=0; i < elements; ++i) {
    list.push_back(i);
  }

  std::random_shuffle(list.begin(), list.end());

    printf("Apply Shuffle\n");
    for (size_t i=1; i < list.size(); ++i)
    {
        data[list[i-1]].n = &data[list[i]];
        data[list[i]].n = NULL;
    }

    // Circle
    data[list.back()].n = &data[list[0]];
}

int main()
{
  printf("Allocating Memory\n");
  test_t* data = (test_t*) malloc(elements * sizeof(test_t));
  printf("Shuffling Array\n");
  fill_random(data);

  printf("Starting Pointer Chasing\n");
  #pragma omp parallel
  {
    int i = 0;
    while(i < times) {
      HUNDRED;
      i++;
    }
  }
  return 0;
}