#!/usr/bin/env ruby
# frozen_string_literal: true

require 'yaml'

functions = YAML.load_file('functions.yaml', symbolize_names: true, freeze: true)
functions.each do |function|
  name = function[:name]
  arguments_with_type = function[:arguments]
  increment_comm_on_enter = function[:increment_comm_on_enter]
  decrement_comm_on_exit = function[:decrement_comm_on_exit]

  arguments = arguments_with_type
              .split(',')
              .filter_map { |arg_t| Regexp.last_match(1) if arg_t =~ /([^*\[\] ]+)(?:\[\])?$/ }
              .join(',')

  puts <<~FUNCTION
    int #{name}(#{arguments_with_type}) {
      int result, world_rank;
      MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

      #{'send_event(+1, world_rank);' if increment_comm_on_enter}
      result = P#{name}(#{arguments});
      #{'send_event(-1, world_rank);' if decrement_comm_on_exit}

      return result;
    }
  FUNCTION
end
