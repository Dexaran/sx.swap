void swapSx::update_spot_prices( const symbol_code base )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    swapSx::spotprices_table _spotprices( get_self(), get_self().value );
    auto spotprices = _spotprices.get_or_default();

    // spot prices
    for ( const auto token : _tokens ) {
        const symbol_code quote = token.sym.code();
        spotprices.quotes[quote] = get_spot_price( base, quote );
    }

    // save table
    spotprices.last_modified = current_time_point();
    spotprices.base = base;
    _spotprices.set( spotprices, get_self() );
}

double swapSx::get_spot_price( const symbol_code base, const symbol_code quote )
{
    const vector<double> uppers = get_uppers( get_self(), base, quote );
    return uppers[0] / uppers[1];
}