

    // player - 100
	// conductor - 200 
	// reporter - 300
	// baron - 400
	// detective - 500
	// starlet - 600
	// valet - 700
	// professor - 800

#define HAS_MET 1


loadDialog(101, 201, 0,
    STR("Hello."));

loadDialog(201, 102, HAS_MET,
    STR("Hello. I hope you are enjoying the journey.\nIs there anything I can do for you?"));

loadDialog(102, 0, 0, 
    STR("No, thankyou."));


// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
