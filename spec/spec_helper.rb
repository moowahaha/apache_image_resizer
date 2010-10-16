require 'lib/resize_wrapper'

RSpec.configure do |config|
  config.before(:all) do
    @resizer = ResizeWrapper.new
  end
end