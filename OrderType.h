#pragma once

enum class OrderType
{
	GoodForDay, // Expires at end of day
    InsertOrCancel, // Fill And Kill  
};