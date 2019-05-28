module pdaf


use iso_c_binding, only: c_int, c_double

implicit none

contains


subroutine c_init_parallel_pdaf(dim_ens, screen) bind(c)
  integer(c_int), intent(in) :: dim_ens, screen

  call init_parallel_pdaf(dim_ens, screen)

end subroutine c_init_parallel_pdaf


subroutine c_init_model(time_steps, field, x_no) bind(c)
  real(c_double), dimension(x_no), intent(inout) :: field
  integer(c_int), intent(in) :: time_steps, x_no

  call init_model(time_steps, field, x_no)

end subroutine c_init_model


subroutine c_init_pdaf(ens_no) bind(c)
  integer(c_int), intent(in) :: ens_no

  call init_pdaf(ens_no)

end subroutine c_init_pdaf


subroutine c_assimilate_pdaf() bind(c)
  call assimilate_pdaf()
end subroutine c_assimilate_pdaf


subroutine c_finalize_pdaf() bind(c)
  call finalize_pdaf()
end subroutine c_finalize_pdaf

integer(c_int) function c_get_rank() bind(c)

  call get_rank(c_get_rank)

end function c_get_rank


end module pdaf
