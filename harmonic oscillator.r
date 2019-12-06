library(lattice)
library(deSolve)

# This turned out not to work well because it doesn't to tend toward a specific amplitude.
oscillator = function(t, state, parms) {
    gamma = 0  # No friction to dampen oscillation. # 0.15
    dstate=numeric(2)
    dstate[1] = state[2]
    dstate[2] = -gamma * state[2] - state[1]
    force = min(3, 3 * (1-state[1])) * (state[1] < 0.6 && state[1] > 0.3 && state[2] > 0)
    force = 0

    dstate[2] = dstate[2] + force

    list(dstate, force)
}

istate = c(1.05, 1.03)
result = lsodes(istate, seq(0,  30*pi, length=1000), oscillator)

x11(); xyplot(`1` + `2` + `3` ~ time, as.data.frame(result), type='l', auto=TRUE)

# This appears to decay continuously.
oscillator = function(t, state, parms) {
    dstate=numeric(2)
    dstate[1] = state[2]^2 - 2*state[1]
    dstate[2] = state[2]^2 - state[1]*state[2]
    force = 0
    if (t > 10 & t < 11) force = -0.05 * sign(dstate[2])
    dstate[2] = dstate[2] + force
    list(dstate, force)
}

istate = c(1.05, 1.03)
result = lsodes(istate, seq(0,  1000*pi, length=10000), oscillator)

x11(); xyplot(`1` + `2` + `3` ~ time, as.data.frame(result), type='l', auto=TRUE)

# This is roughly like a bouncing ball, but the way the force is applied makes it behave strangly. Compare forces of 20, 100 and 200.
# This behaves strangly.

d^2h/dt^2 = g


bouncer = function(t, state, parms) {
    dstate = numeric(2)
    dstate[1] = state[2]
    dstate[2] = -9.8

    force = 0
    if ( state[1] < 0.1 ) {
        force = 20
        #dstate[1] = abs(dstate[1])
    }

    dstate[2] = dstate[2] + force

    list(dstate, force)
}

istate = c(2, 0)
result = lsodes(istate, seq(0,  3*pi, length=1000), bouncer)

x11(); plot(result[, '1'] ~ result[, 'time'], col='black', type='l', ylim=c(-6, 6))
lines(result[, '2'] ~ result[, 'time'], col='red', type='l')
lines(result[, '3'] ~ result[, 'time'], col='blue', type='l')
abline(h=0, lty=2, col='grey')


# This is similar to enzyme kinetics, and it oscillates briefly, but damples quickly
# Perhaps there are parameters that cause stable oscillations, but I couldn't find any.
decayer = function(t, state, parms) {
    dstate = numeric(2)
    growth_rate = 2 * state[2]
    decay_coef1 = 1
    dstate[1] = growth_rate -decay_coef1 * state[1]

    decay_coef2 = 2 * state[1] + 1
    dstate[2] = 4 * state[2] - decay_coef2 * state[2]

    force = 0
    if ( state[1] < 1 ) {
        force = 10 * (1 - state[1])
    }

    dstate[2] = dstate[2] + force

    list(dstate)
}

istate = c(2, 1)
result = lsodes(istate, seq(0,  3*pi, length=100), decayer)

#x11(); plot(result, ylim=c(-1, 1))
x11(); plot(result[, '1'] ~ result[, 'time'], col='black', type='l', ylim=c(-1, 3))
lines(result[, '2'] ~ result[, 'time'], col='red', type='l')
abline(h=0, lty=2, col='grey')


# This is from the file listed below, but the file is now gone. It oscillates stably, and returns to the same period when perturbed.
# The fourth order growth is bound to cause problems with a numerical solver.
# It also seems to stable - it requires a very large force to cause it to shift periods.
# http://student.ulb.ac.be/~dgonze/TEACHING/osc_design.pdf
relaxation_oscillator = function(t, state, parms) {
    dstate=numeric(2)
    state[2] = max(state[2], 0)
    n=4
    p=200
    a1=10
    a2=2
    b1=1/2
    b2=2
    gamma=10
    growth = (1 + p * state[2]^n)/(1 + state[2]^n)
    dstate[1] = a2 * growth - b2 * state[1]
    dstate[2] = a1 * growth - b1 * state[2] - gamma*state[1]*state[2]
    force = 0
    if (t > 16 & t < 17) force = 170
    dstate[2] = dstate[2] + force
    list(dstate, c(force=force))
}

istate = c(1.05, 1.03)
result = lsodes(istate, seq(0,  10*pi, length=1000), relaxation_oscillator)

#x11(); plot(result, ylim=c(-1, 1))
x11(); xyplot(`1` + `2` + force ~ time, as.data.frame(result), type='l', auto=TRUE)

subset(as.data.frame(result), time > 9.9 & time < 10.1)

