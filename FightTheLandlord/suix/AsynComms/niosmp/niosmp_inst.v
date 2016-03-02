	niosmp u0 (
		.clk_clk          (<connected-to-clk_clk>),          //       clk.clk
		.reset_reset_n    (<connected-to-reset_reset_n>),    //     reset.reset_n
		.leds_export      (<connected-to-leds_export>),      //      leds.export
		.data_export      (<connected-to-data_export>),      //      data.export
		.address_export   (<connected-to-address_export>),   //   address.export
		.rnw_export       (<connected-to-rnw_export>),       //       rnw.export
		.noe_export       (<connected-to-noe_export>),       //       noe.export
		.mpdatain_export  (<connected-to-mpdatain_export>),  //  mpdatain.export
		.chrec_export     (<connected-to-chrec_export>),     //     chrec.export
		.mpdataout_export (<connected-to-mpdataout_export>), // mpdataout.export
		.asoe_export      (<connected-to-asoe_export>),      //      asoe.export
		.sent_export      (<connected-to-sent_export>),      //      sent.export
		.load_export      (<connected-to-load_export>),      //      load.export
		.testin_export    (<connected-to-testin_export>)     //    testin.export
	);

