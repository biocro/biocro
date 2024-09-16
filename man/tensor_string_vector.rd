\name{tensor_string_vector}
\alias{tensor_string_vector}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
    Generate the tensor product of two string vectors.
}
\description{
    This function is a utility function to be used wtih \code{\link{module_write}}. It returns the tensor product or Cartesian product of two string vectors.
    }
\usage{
    tensor_string_vector(x, y, sep="_", order_by_left_first=TRUE)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
    \item{x}{
        The left string vector. The elements of the left vector will appear on the left in the result.
    }
    \item{y}{
        The right string vector.  The elements of the right vector will appear on the right in the result.
    }
    \item{sep}{
        This string will be used to separate left and right strings.
    }
    \item{order_by_left_first}{
        If true, the result is ordered so that first element of the left vector is paired with all right elements first before the second left element. If false, the left elements will
    }
}

\value{
    A string vector which is the tensor product of the inputs.
}
\note{
    Given a left string vector \code{c("A", "B", ...)} and a right string vector \code{c("1", "2")}, the tensor product is a vector which contains all ordered pairs separated by \code{sep}: \code{c("A_1", "A_2", "B_1", "B_2", ...)}. The vector will have dimension that is the product of the input vectors.
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{

# example one; all pairs of upper and lowercase letters.
tensor_string_vector(LETTERS,letters)

# first five capital letters paired with integers 1-3. No separator
tensor_string_vector(letters[1:5], 1:3, sep="")

# same but with `.` as the separator and ordered by the right.
tensor_string_vector(letters[1:5], 1:3, sep=".",FALSE)

# concrete biocro example
organs = c("leaf", "stem", "root")
pools = c("carbon", "nitrogen")
tensor_string_vector(organs, pools)

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
% \keyword{ ~kwd1 }
% \keyword{ ~kwd2 }
% Use only one keyword per line.
% For non-standard keywords, use \concept instead of \keyword:
% \concept{ ~cpt1 }
% \concept{ ~cpt2 }
% Use only one concept per line.
