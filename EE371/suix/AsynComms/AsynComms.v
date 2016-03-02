module AsynComms(clk, key, led, datain, dataout, db2mp,newdone,sent); output newdone,sent;
	input clk;
	//input[7:0] sw;
	input[0:0] key;
	
	input datain;
	output dataout, db2mp;
	
	output[7:0] led;
	wire[7:0] address;
	wire[7:0] data;
	
	//
	wire datain;
	wire[3:0] bsincounter;
	wire[3:0] bsoutcounter;
	wire[9:0] db2mp;
	wire[7:0] mp2db;
	
	reg[23:0] tbase;
	
	wire newdone,sent;

	always @(posedge clk)
		tbase = tbase + 1;
	
	sraminner mys(data,address,noe,rnw,tbase[22]);

	niosmp u0 (
		.clk_clk          (clk),				//      clk.clk
		.reset_reset_n    (key),				//    reset.reset_n
		.leds_export      (led),				//     leds.export
		.data_export      (data),				//     data.export
		.address_export   (address),			//  address.export
		.rnw_export       (rnw),				//      rnw.export
		.noe_export       (noe),				//      noe.export
		.mpdatain_export  (db2mp[8:1]), 			// mpdatain.export
      .chrec_export     (newdone),				//    chrec.export
      .mpdataout_export (mp2db),				//mpdataout.export
      .asoe_export      (asoe),				//     asoe.export
      .sent_export      (sent),				//     sent.export
      .load_export      (load),				//     load.export
		.testin_export    (dataout)
	);
	
	//in
	SBD mysbd(inenable, datain, done);
	//SBD mysbd(inenable, dataout, done);
	//BSC bscin(bsincounter, clk, inenable);
	BSC bscin(bsincounter, tbase[9], inenable);
	BIC bicin(done, bsincounter, inenable);
	SRCC srccin(sample, bsincounter);
	S2P mys2p(db2mp, datain, sample);
	//S2P mys2p(db2mp, dataout, sample);
	
	try try1(newdone,done,tbase[10]);  // this is tryyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy
	
	//out
	//BSC bscout(bsoutcounter, clk, asoe);
	BSC bscout(bsoutcounter, tbase[9], asoe);
	BIC bicout(sent, bsoutcounter, asoe);
	SRCC srccout(signal, bsoutcounter);
	P2S myp2s(dataout, mp2db, signal, load);
	
endmodule

module try(newdone,done,clk);
output newdone;
input done,clk;
reg newdone;

always @(posedge done or negedge clk)
begin
	if(done)
		newdone = 1;
	else
		newdone = 0;
end

endmodule

module BSC(counter, clk, enable);
	output[3:0] counter;
	input clk, enable;
	reg[3:0] counter;
	
	always @(posedge clk or negedge enable) begin
		if (!enable)
			counter=0;
		else
			counter=counter+1;
	end

endmodule

module BIC(done, bscounter, enable);
	input enable;
	input[3:0] bscounter;
	output done;
	reg[3:0] counter; //done when 1010
	wire sample;

	assign sample=bscounter[3] & (~bscounter[2]) & (bscounter[1]) & (~bscounter[0]); //count at bsc=1010
	assign done=counter[3] & (~counter[2]) & counter[1] &(~counter[0]);
	
	always @(posedge sample or negedge enable) begin
		if (!enable)
			counter=0;
		else
			counter=counter+1;
	end

endmodule

module SBD(enable, data, rst);
	output enable;
	input data, rst;
	reg enable;

	always@(posedge rst or negedge data) begin
		if (rst)
			enable=0;
		else
			enable=1;
	end
	
endmodule

module SRCC(sample, bscounter);
	output sample;
	input[3:0] bscounter;
	assign sample=(~bscounter[3]) & bscounter[2] & bscounter[1] & bscounter[0]; //sample at 0111
endmodule

module S2P(db2mp, datain, clk); //clk=sample
	output[8:1] db2mp;
	input clk, datain;
	reg[9:0] db2mp;
	
	always@(posedge clk)
			db2mp[9:0]={db2mp[8:0], datain};
endmodule

module P2S(dataout, mp2db, clk, load);
	output dataout;
	input[7:0] mp2db;
	input clk, load;
	reg[9:0] sreg;
	
	assign dataout=sreg[9];
	
	always@(posedge load or posedge clk) begin
		if (load)
			sreg={2'b10, mp2db};
		else
			sreg[9:0]={sreg[8:0], 1'b1};
	end
	
endmodule

module sraminner(data,address,noe,rnw,tbase);
  
  inout [7:0] data;
  input tbase;
  input [7:0] address;
  input noe,rnw;
  
  reg [7:0] memory [255:0];
  reg [7:0] data_out;
  
  assign data = (!noe) ? data_out:8'bzzzzzzzz;
  
  always 
	begin
		
		if(!noe && rnw) //read
			data_out = memory[address];
		
		if(noe && !rnw) //write
			memory[address] = data;


	end
endmodule
