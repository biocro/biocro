get_all_modules <- function()
{
	.Call(R_get_all_modules)
}

get_all_quantities <- function()
{
	as.data.frame(.Call(R_get_all_quantities))
}

get_all_ode_solvers <- function()
{
	.Call(R_get_all_ode_solvers)
}

get_all_se_solvers <- function()
{
	.Call(R_get_all_se_solvers)
}
