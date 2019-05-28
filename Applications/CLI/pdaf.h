extern void c_init_parallel_pdaf(int* dim_ens, int* screen);
extern void c_init_model(int* time_steps, double* data, int* x_no);
extern void c_init_pdaf(int* ens_no);
extern void c_assimilate_pdaf();
extern void c_finalize_pdaf();
extern int  c_get_rank();
