#include <stdlib.h>
#include "m_pd.h"

#define MAX_BUFFER_LENGTH 8192

static t_class *percentile_class;

typedef struct {
  t_object  x_obj;
  t_float buffer[MAX_BUFFER_LENGTH];
  size_t buffer_length;
  size_t index;
} Percentile;

void percentile_calc(Percentile *x, t_floatarg new_value)
{
  // Calculate and send percentile
  t_int larger_values = 0;
  for(size_t i = 0; i < x->buffer_length; i++)
  {
    if(new_value > x->buffer[i])
    {
      larger_values++;
    }
  }
  outlet_float(x->x_obj.ob_outlet, ((t_float) larger_values) / x->buffer_length);

  // Cyclic insert
  x->buffer[x->index] = new_value;
  x->index = (x->index + 1) % x->buffer_length;
}

void *percentile_new(t_floatarg buffer_length)
{
  if (buffer_length > MAX_BUFFER_LENGTH)
  {
    error("percentile: buffer_length out of range, using %d", MAX_BUFFER_LENGTH);
    buffer_length = MAX_BUFFER_LENGTH;
  }

  Percentile *x = (Percentile *) pd_new(percentile_class);

  x->buffer_length = buffer_length;
  x->index = 0;
  for(size_t i = 0; i < x->buffer_length; i++)
  {
    x->buffer[i] = 0;
  }

  outlet_new(&x->x_obj, &s_float);

  return (void *) x;
}

void percentile_setup(void) {
  percentile_class = class_new(gensym("percentile"),
                               (t_newmethod) percentile_new,
                               0,
                               sizeof(Percentile),
                               CLASS_DEFAULT,
                               A_DEFFLOAT,  // buffer_length
                               0);

  class_addfloat(percentile_class, percentile_calc);
}
