#pragma once
#define SIGNAL(signal, SIGNAL_FUNCTION)\
	if (signal)\
	{\
		SIGNAL_FUNCTION;\
		signal = false;\
	}
#define SIGNAL_CONDITIONAL(signal, conditional, function)\
	if (signal && conditional)\
	{\
		function;\
		signal = false;\
	}
#define DISABLE_SIGNAL(signal)\
	signal ? signal = !signal : signal = signal;