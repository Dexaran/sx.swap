bool swapSx::is_token_exists( const symbol_code symcode )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    return ( _tokens.find( symcode.raw() ) != _tokens.end() );
}

double swapSx::get_ratio( const symbol_code symcode )
{
    // calculate ratio between depth & balance
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    const asset balance = swapSx::get_balance( symcode );
    return static_cast<double>(balance.amount) / token.depth.amount;
}

void swapSx::check_remaining_balance( const asset out )
{
    const asset balance = get_balance( out.symbol.code() );
    const asset depth = get_depth( out.symbol.code() );
    const asset remaining = balance - out;

    check( remaining.amount >= 0, out.symbol.code().to_string() + " insufficient remaining balance" );
}

void swapSx::check_is_active( const symbol_code symcode, const name contract )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " token not available");

    if ( contract.value ) check( contract == token->contract, symcode.to_string() + " contract mismatch");
}