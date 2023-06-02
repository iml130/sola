# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from enum import Enum


class MessageType(Enum):
    # General
    INIT = 0
    PAYLOAD = 1

    # Join Procedure
    JOIN = 10
    JOIN_ACCEPT = 12
    JOIN_ACCEPT_ACK = 14

    # Entity Search
    FIND_QUERY_REQUEST = 20
    FIND_QUERY_ANSWER = 22
    ATTRIBUTE_INQUIRY_REQUEST = 24
    ATTRIBUTE_INQUIRY_ANSWER = 26
    SUBSCRIPTION_ORDER = 28
    SUBSCRIPTION_UPDATE = 30

    # Search Exact
    SEARCH_EXACT = 40
    SEARCH_EXACT_FAILURE = 41
    EMPTY = 42

    # Bootstrap Algorithm
    BOOTSTRAP_DISCOVER = 50
    BOOTSTRAP_RESPONSE = 52

    # Updates & Response Algorithm
    # Messages used for both Join & Leave
    REMOVE_NEIGHBOR = 60
    REMOVE_NEIGHBOR_ACK = 62
    UPDATE_NEIGHBORS = 64
    REPLACEMENT_UPDATE = 66
    GET_NEIGHBORS = 70
    INFORM_ABOUT_NEIGHBORS = 72

    # Leave Procedure
    FIND_REPLACEMENT = 80
    REPLACEMENT_NACK = 81
    SIGN_OFF_PARENT_REQUEST = 82
    LOCK_NEIGHBOR_REQUEST = 84
    LOCK_NEIGHBOR_RESPONSE = 86
    SIGN_OFF_PARENT_ANSWER = 88
    REMOVE_AND_UPDATE_NEIGHBOR = 90
    REPLACEMENT_OFFER = 92
    REPLACEMENT_ACK = 94
    UNLOCK_NEIGHBOR = 96


class MessageProcessingModes(Enum):
    RECEIVING = 0
    SENDING = 1


class SearchExactTestEntryTypes(Enum):
    START = 0
    HOP = 1
    SUCCESS = 2
    FAILURE = 3


class NeighborRelationship(Enum):
    PARENT = 0
    CHILD = 1
    ADJACENT_LEFT = 2
    ADJACENT_RIGHT = 3
    ROUTING_TABLE_NEIGHBOR = 4
    ROUTING_TABLE_NEIGHBOR_CHILD = 5
    UNKNOWN = 6


class NodeState(Enum):
    UNINIT = 0
    RUNNING = 1
    LEFT = 2
    FAILED = 3


class NodeContentStatus(Enum):
    INSERTED_OR_UPDATED = 0
    REMOVED = 1


class EventTypes(Enum):
    JOIN = 0
    LEAVE = 1
    FIND_QUERY = 3
    REQUEST_COUNTDOWN_START = 4


class ComparisonType(Enum):
    EQUAL_TO = 0
    NOT_EQUAL_TO = 1
    LESS_THAN = 2
    GREATER_THAN = 3
    LESS_THAN_OR_EQUAL_TO = 4
    GREATHER_THAN_OR_EQUAL_TO = 5
