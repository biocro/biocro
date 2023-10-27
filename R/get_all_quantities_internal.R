get_all_quantities_internal <- function()
{
	as.data.frame(.Call(R_get_all_quantities), stringsAsFactors = FALSE)
}
