[[eosio::on_notify("*::transfer")]]
void swapSx::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // authenticate incoming `from` account
    require_auth( from );

    // ignore transfers
    const set<name> ignore = set<name>{
        // EOSIO system accounts
        "eosio.stake"_n,
        "eosio.names"_n,
        "eosio.ram"_n,
        "eosio.rex"_n,
        "eosio"_n,
    };

    // ignore transfers
    if ( memo == get_self().to_string() ) return;
    if ( to != get_self() ) return;
    if ( ignore.find( from ) != ignore.end() ) return;

    // check if contract maintenance is ongoing
    swapSx::settings _settings( get_self(), get_self().value );
    check( _settings.exists(), "contract is currently disabled for maintenance");

    // validate input
    const symbol_code in_symcode = quantity.symbol.code();
    const symbol_code out_symcode = parse_memo_symcode( memo );
    check_is_active( in_symcode, get_first_receiver() );
    check_is_active( out_symcode, name{} );
    check( in_symcode != out_symcode, in_symcode.to_string() + " symbol code cannot be the same as quantity");

    // calculate rates
    const asset fee = swapSx::get_fee( get_self(), quantity );
    const asset rate = swapSx::get_rate( get_self(), quantity, out_symcode );

    // validate output
    check( rate.amount > 0, "quantity must be higher");
    check_remaining_balance( rate );

    // send transfers
    self_transfer( from, rate, "convert" );
    if ( fee.amount ) self_transfer( "fee.sx"_n, fee, "fee" );

    // post transfer
    update_volume( vector<asset>{ quantity, rate }, fee );

    // update balances `on_notify` inline transaction
    // prevents re-entry exploit
    add_balance( quantity - fee );
    sub_balance( rate );
    update_spot_prices();

    // trade log
    const double trade_price = asset_to_double( rate ) / asset_to_double( quantity );
    const double spot_price = get_spot_price( _settings.get().base, rate.symbol.code() );
    const double value = spot_price * asset_to_double( rate );

    swapSx::log_action log( get_self(), { get_self(), "active"_n });
    log.send( from, quantity, rate, fee, trade_price, spot_price, value );
}