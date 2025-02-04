\name{Time Variable}

\alias{Time Variable}
\alias{time}

\title{The `time` variable in BioCro}

\description{
  Even when using an adaptive ODE solver, \code{run_biocro} returns values at
  evenly spaced time intervals given by the `timestep` parameter. It is assumed
  that the drivers are provided at intervals spaced by this `timestep`. To prevent
  user error, \code{run_biocro} will check that the `drivers`` are spaced by `timestep`
  but to do so, the `drivers` must contain a `time`.
  }

\details{
    When differential modules are passed, BioCro will check that the drivers
    have a `time` variable that satisfies: `time[n+1] - time[n] = timestep`
    for all `n` (no checks are required for direct_modules). BioCro assumes
    that differential modules return rates-of-change in the same units as
    `timestep`.

    Unless using modules which require time to correpond to a calendar time
    (a date and time), the starting `time[1]` has no special meaning.
    If `time[1] = 0` then `time` is the amount of time that has passed
    since the simulation's start (in the same units as `timestep`).

    Certain modules expect `time` to specify a date and time with respect to the
    calendar. For most BioCro simulations, the rates of change are measured
    per hour, therefore `timestep` has units hour.

    Therefore, the time variable is defined as the number of hours that
    have passed since midnight January 1st of the same year. For example
    for year 2023:
     + `time = 0` is '2023-01-01 00:00:00'
     + `time = 1` is '2023-01-01 01:00:00'
     + `time = 50`is '2023-01-03 02:00:00'
    Note in version 3.1.3 and earlier of BioCro `time` counted the number
    of days rather than hours.
}
