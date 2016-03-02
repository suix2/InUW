	component niosmp is
		port (
			clk_clk          : in    std_logic                    := 'X';             -- clk
			reset_reset_n    : in    std_logic                    := 'X';             -- reset_n
			leds_export      : out   std_logic_vector(7 downto 0);                    -- export
			data_export      : inout std_logic_vector(7 downto 0) := (others => 'X'); -- export
			address_export   : out   std_logic_vector(7 downto 0);                    -- export
			rnw_export       : out   std_logic;                                       -- export
			noe_export       : out   std_logic;                                       -- export
			mpdatain_export  : in    std_logic_vector(7 downto 0) := (others => 'X'); -- export
			chrec_export     : in    std_logic                    := 'X';             -- export
			mpdataout_export : out   std_logic_vector(7 downto 0);                    -- export
			asoe_export      : out   std_logic;                                       -- export
			sent_export      : in    std_logic                    := 'X';             -- export
			load_export      : out   std_logic;                                       -- export
			testin_export    : in    std_logic                    := 'X'              -- export
		);
	end component niosmp;

	u0 : component niosmp
		port map (
			clk_clk          => CONNECTED_TO_clk_clk,          --       clk.clk
			reset_reset_n    => CONNECTED_TO_reset_reset_n,    --     reset.reset_n
			leds_export      => CONNECTED_TO_leds_export,      --      leds.export
			data_export      => CONNECTED_TO_data_export,      --      data.export
			address_export   => CONNECTED_TO_address_export,   --   address.export
			rnw_export       => CONNECTED_TO_rnw_export,       --       rnw.export
			noe_export       => CONNECTED_TO_noe_export,       --       noe.export
			mpdatain_export  => CONNECTED_TO_mpdatain_export,  --  mpdatain.export
			chrec_export     => CONNECTED_TO_chrec_export,     --     chrec.export
			mpdataout_export => CONNECTED_TO_mpdataout_export, -- mpdataout.export
			asoe_export      => CONNECTED_TO_asoe_export,      --      asoe.export
			sent_export      => CONNECTED_TO_sent_export,      --      sent.export
			load_export      => CONNECTED_TO_load_export,      --      load.export
			testin_export    => CONNECTED_TO_testin_export     --    testin.export
		);

