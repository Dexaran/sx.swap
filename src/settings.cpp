[[eosio::action]]
void stable::setparams( const optional<stable::params> params )
{
    require_auth( get_self() );
    stable::settings _settings( get_self(), get_self().value );
    stable::docs_singleton _docs( get_self(), get_self().value );

    if ( !params ) {
        _docs.remove();
        _settings.remove();
        return;
    }
    _docs.get_or_create( get_self() );

    check( params->fee <= 50, "fee cannot be greater than 0.5%");
    check( params->fee >= 0, "fee must be positive");
    check( params->amplifier <= 100, "amplifier cannot be greater than 100x");
    check( params->amplifier >= 0, "amplifier must be positive");

    _settings.set( *params, get_self() );
}

[[eosio::action]]
void stable::token( const symbol_code symcode, const optional<name> contract )
{
    require_auth( get_self() );

    stable::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( symcode.raw() );

    // delete if contract is null
    if ( itr != _tokens.end() && contract->value == 0 ) {
        _tokens.erase( itr );
        return;
    }

    check( itr == _tokens.end(), "token already exists");

    // validate symcode & contract
    check( contract->value, "contract is required");
    const asset supply = token::get_supply( *contract, symcode );
    const asset balance = token::get_balance( *contract, get_self(), symcode );
    if ( contract ) check( supply.amount > 0, "invalid supply");

    _tokens.emplace( get_self(), [&]( auto& row ){
        row.sym = supply.symbol;
        row.contract = *contract;
        row.balance = balance;
        row.depth = balance;
    });
}