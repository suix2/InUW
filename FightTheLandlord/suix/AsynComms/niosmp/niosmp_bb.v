
module niosmp (
	clk_clk,
	reset_reset_n,
	leds_export,
	data_export,
	address_export,
	rnw_export,
	noe_export,
	mpdatain_export,
	chrec_export,
	mpdataout_export,
	asoe_export,
	sent_export,
	load_export,
	testin_export);	

	input		clk_clk;
	input		reset_reset_n;
	output	[7:0]	leds_export;
	inout	[7:0]	data_export;
	output	[7:0]	address_export;
	output		rnw_export;
	output		noe_export;
	input	[7:0]	mpdatain_export;
	input		chrec_export;
	output	[7:0]	mpdataout_export;
	output		asoe_export;
	input		sent_export;
	output		load_export;
	input		testin_export;
endmodule
