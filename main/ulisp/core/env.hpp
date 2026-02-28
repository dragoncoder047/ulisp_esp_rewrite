#include "types.hpp"

extern object* GlobalEnv;
extern object* tee;

object* value(object* name, object* env);

void initenv();
