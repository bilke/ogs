extern "C" void c_init_parallel_pdaf(int* dim_ens, int* screen);
extern "C" void c_init_model(int* time_steps, double* data, int* x_no);
extern "C" void c_init_pdaf(int* ens_no);
extern "C" void c_assimilate_pdaf();
extern "C" void c_finalize_pdaf();
extern "C" int  c_get_rank();
