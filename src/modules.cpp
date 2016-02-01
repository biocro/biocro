

#include "modules.h"

IModule::list_required_state() {
	return(this->_required_state)
}

IModule::list_modified_state() {
	return(this->_modified_state)
}
