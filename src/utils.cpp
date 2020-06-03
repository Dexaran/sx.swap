#include <math.h>

symbol_code stable::parse_memo_symcode( const string memo )
{
    const string error = "`memo` must be symbol code (ex: \"USDT\")";
    check( memo.length() > 0 && memo.length() <= 7 , error );

    // must be all uppercase alpha characters
    for (char const c: memo ) {
        check( isalpha(c) && isupper(c), error );
    }
    const symbol_code symcode = symbol_code{ memo };
    check( symcode.is_valid(), error );
    return symcode;
}

double stable::asset_to_double( const asset quantity )
{
    if ( quantity.amount == 0 ) return 0.0;
    return quantity.amount / pow(10, quantity.symbol.precision());
}

asset stable::double_to_asset( const double amount, const symbol sym )
{
    return asset{ static_cast<int64_t>(amount * pow(10, sym.precision())), sym };
}

void stable::self_transfer( const name to, const asset quantity, const string memo )
{
    token::transfer_action transfer( get_contract( quantity.symbol.code() ), { get_self(), "active"_n });
    transfer.send( get_self(), to, quantity, memo );
}