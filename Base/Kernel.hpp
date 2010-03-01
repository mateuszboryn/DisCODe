/*!
 * \file Kernel.hpp
 * \brief
 *
 * \author mstefanc
 * \date 2010-02-26
 */

#ifndef KERNEL_HPP_
#define KERNEL_HPP_

#include <string>
#include <vector>

#include "../Core/DataStreamInterface.hpp"

namespace Base {

/*!
 * \class Kernel
 * \brief
 *
 * \author mstefanc
 */
class Kernel {
public:
	Kernel(std::string n = "unnamed_kernel") : name(n) {};

	virtual ~Kernel() {};

protected:
	/*!
	 * Register new data stream for this object.
	 */
	void registerDataStream(Core::DataStreamInterface * ds) {
		streams.push_back(ds);
	}

	/*!
	 * Return list of all registered data streams.
	 */
	const std::vector<Core::DataStreamInterface *> & getDataStreams() const {
		return streams;
	}


private:
	/*!
	 * List of all data streams created for particular kernel module.
	 */
	std::vector<Core::DataStreamInterface *> streams;

	/*!
	 * Name of this module. It's used for runtime identification.
	 */
	std::string name;
};

}

#endif /* KERNEL_HPP_ */