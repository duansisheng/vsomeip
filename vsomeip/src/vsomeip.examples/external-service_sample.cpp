#include <iomanip>
#include <iostream>

#include <vsomeip/vsomeip.hpp>

using namespace vsomeip;

#define EXTERNAL_SAMPLE_SERVICE			 	0x1234
#define EXTERNAL_SAMPLE_SERVICE_INSTANCE	0x2356
#define EXTERNAL_SAMPLE_METHOD			 	0x0203

factory *the_factory = 0;
application *the_application = 0;

void receive_message(const message_base *_message) {
	static int i = 0;

	std::cout << "[" << std::dec << std::setw(4) << std::setfill('0') << i++
			  << "] Application "
			  << std::hex << _message->get_client_id()
			  << " sends a request with "
			  << std::dec << _message->get_length() << " bytes."
			  << std::endl;

	message *response = the_factory->create_response(_message);

	uint8_t payload_data[] = { 0x11, 0x22, 0x44, 0x66, 0x88 };
	response->get_payload().set_data(payload_data, sizeof(payload_data));

	the_application->send(response);
}

int main(void) {
	the_factory = factory::get_instance();
	endpoint *location = the_factory->get_endpoint("127.0.0.1", 30498, ip_protocol::TCP);

	// create the application and provide a service at the defined location
	the_application = the_factory->create_managing_application("SimpleServiceApplication");

	the_application->provide_service(EXTERNAL_SAMPLE_SERVICE, EXTERNAL_SAMPLE_SERVICE_INSTANCE, location);
	the_application->register_cbk(EXTERNAL_SAMPLE_SERVICE, EXTERNAL_SAMPLE_SERVICE_INSTANCE, EXTERNAL_SAMPLE_METHOD, receive_message);

	the_application->start();
	while (1) the_application->run();
}
