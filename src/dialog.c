

    // player - 100
	// conductor - 200 
	// reporter - 300
	// baron - 400
	// detective - 500
	// starlet - 600
	// valet - 700
	// professor - 800



loadDialog(0, 0, 0, STR("I am Error"));

loadDialog(101, 201, 0,
    STR("Hello."));

loadDialog(201, 102, 0,
    STR("Hello. I hope you are enjoying the \njourney.\nIs there anything I can do for you?"));

loadDialog(102, 0, HAS_MET, 
    STR("No, thankyou."));

loadDialog(202, 0, 0, STR("Hmmpf"));

// if (HAS_MET) // set new dialogID and set flag back to zero?



// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
// loadDialog(101, 201, STR("Hello."));
